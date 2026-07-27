#!/usr/bin/env python3
"""Small stdio MCP server for exercising Notaryze signing flows on macOS."""

from __future__ import annotations

import json
import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path
from typing import Any


PROTOCOL_VERSION = "2024-11-05"


def run(argv: list[str]) -> dict[str, Any]:
    try:
        proc = subprocess.run(argv, text=True, capture_output=True, check=False)
        return {
            "exit_code": proc.returncode,
            "stdout": proc.stdout,
            "stderr": proc.stderr,
            "command": argv,
        }
    except FileNotFoundError as exc:
        return {
            "exit_code": 127,
            "stdout": "",
            "stderr": str(exc),
            "command": argv,
        }


def require_path(args: dict[str, Any]) -> Path:
    value = args.get("path")
    if not isinstance(value, str) or not value:
        raise ValueError("path is required")

    path = Path(value).expanduser().resolve()
    if not path.exists():
        raise ValueError(f"path does not exist: {path}")

    return path


def verify_signature(args: dict[str, Any]) -> dict[str, Any]:
    path = require_path(args)
    verify = run(["codesign", "--verify", "--deep", "--strict", "--verbose=4", str(path)])
    details = run(["codesign", "-dv", "--verbose=4", str(path)])
    detail_text = details["stdout"] + details["stderr"]

    return {
        "path": str(path),
        "signed": verify["exit_code"] == 0,
        "verify": verify,
        "details": details,
        "authorities": [
            line.removeprefix("Authority=")
            for line in detail_text.splitlines()
            if line.startswith("Authority=")
        ],
        "team_identifier": next(
            (
                line.removeprefix("TeamIdentifier=")
                for line in detail_text.splitlines()
                if line.startswith("TeamIdentifier=")
            ),
            None,
        ),
    }


def sign_ad_hoc(args: dict[str, Any]) -> dict[str, Any]:
    path = require_path(args)
    result = run(["codesign", "--force", "--sign", "-", "--timestamp=none", str(path)])
    return {
        "path": str(path),
        "sign": result,
        "verification": verify_signature({"path": str(path)}),
    }


def remove_signature(args: dict[str, Any]) -> dict[str, Any]:
    path = require_path(args)
    result = run(["codesign", "--remove-signature", str(path)])
    return {
        "path": str(path),
        "remove": result,
        "verification": verify_signature({"path": str(path)}),
    }


def sign_developer_id(args: dict[str, Any]) -> dict[str, Any]:
    path = require_path(args)
    name = args.get("name")
    team_id = args.get("team_id")

    if not isinstance(name, str) or not name:
        raise ValueError("name is required")
    if not isinstance(team_id, str) or not team_id:
        raise ValueError("team_id is required")

    identity = f"Developer ID Application: {name} ({team_id})"
    result = run([
        "codesign",
        "--force",
        "--sign",
        identity,
        "--verbose",
        "--deep",
        "--strict",
        "--options=runtime",
        "--timestamp",
        str(path),
    ])

    return {
        "path": str(path),
        "identity": identity,
        "sign": result,
        "verification": verify_signature({"path": str(path)}),
    }


def list_signing_identities(_: dict[str, Any]) -> dict[str, Any]:
    result = run(["security", "find-identity", "-v", "-p", "codesigning"])
    return {
        "identities": result,
    }


def create_sample_executable(args: dict[str, Any]) -> dict[str, Any]:
    output_dir_value = args.get("output_dir")
    output_dir = Path(output_dir_value).expanduser().resolve() if isinstance(output_dir_value, str) and output_dir_value else Path(tempfile.mkdtemp(prefix="notaryze-mcp-"))
    output_dir.mkdir(parents=True, exist_ok=True)

    source = output_dir / "hello.c"
    binary = output_dir / "hello"
    source.write_text(
        '#include <stdio.h>\nint main(void) { puts("hello from notaryze mcp"); return 0; }\n',
        encoding="utf-8",
    )

    compiler = shutil.which("cc") or shutil.which("clang")
    if compiler is None:
        return {
            "path": str(binary),
            "compile": {
                "exit_code": 127,
                "stdout": "",
                "stderr": "No C compiler found",
                "command": ["cc", str(source), "-o", str(binary)],
            },
        }

    compile_result = run([compiler, str(source), "-o", str(binary)])
    return {
        "source": str(source),
        "path": str(binary),
        "compile": compile_result,
        "verification": verify_signature({"path": str(binary)}) if binary.exists() else None,
    }


TOOLS = {
    "list_signing_identities": {
        "description": "List locally available macOS code-signing identities.",
        "inputSchema": {"type": "object", "properties": {}, "additionalProperties": False},
        "handler": list_signing_identities,
    },
    "verify_signature": {
        "description": "Verify whether a file, app bundle, dylib, or plugin is code signed.",
        "inputSchema": {
            "type": "object",
            "properties": {"path": {"type": "string"}},
            "required": ["path"],
            "additionalProperties": False,
        },
        "handler": verify_signature,
    },
    "sign_ad_hoc": {
        "description": "Ad-hoc sign a local file for test verification without Developer ID credentials.",
        "inputSchema": {
            "type": "object",
            "properties": {"path": {"type": "string"}},
            "required": ["path"],
            "additionalProperties": False,
        },
        "handler": sign_ad_hoc,
    },
    "remove_signature": {
        "description": "Remove a code signature from a local file so unsigned status can be tested.",
        "inputSchema": {
            "type": "object",
            "properties": {"path": {"type": "string"}},
            "required": ["path"],
            "additionalProperties": False,
        },
        "handler": remove_signature,
    },
    "sign_developer_id": {
        "description": "Sign a local file with a Developer ID Application identity.",
        "inputSchema": {
            "type": "object",
            "properties": {
                "path": {"type": "string"},
                "name": {"type": "string"},
                "team_id": {"type": "string"},
            },
            "required": ["path", "name", "team_id"],
            "additionalProperties": False,
        },
        "handler": sign_developer_id,
    },
    "create_sample_executable": {
        "description": "Compile a tiny local test executable and report its initial signing status.",
        "inputSchema": {
            "type": "object",
            "properties": {"output_dir": {"type": "string"}},
            "additionalProperties": False,
        },
        "handler": create_sample_executable,
    },
}


def content(payload: Any) -> list[dict[str, str]]:
    return [{"type": "text", "text": json.dumps(payload, indent=2)}]


def handle(request: dict[str, Any]) -> dict[str, Any] | None:
    method = request.get("method")
    request_id = request.get("id")

    if method == "notifications/initialized":
        return None

    try:
        if method == "initialize":
            result = {
                "protocolVersion": PROTOCOL_VERSION,
                "capabilities": {"tools": {}},
                "serverInfo": {"name": "notaryze-mcp", "version": "0.1.0"},
            }
        elif method == "tools/list":
            result = {
                "tools": [
                    {
                        "name": name,
                        "description": tool["description"],
                        "inputSchema": tool["inputSchema"],
                    }
                    for name, tool in TOOLS.items()
                ]
            }
        elif method == "tools/call":
            params = request.get("params") or {}
            name = params.get("name")
            args = params.get("arguments") or {}
            if name not in TOOLS:
                raise ValueError(f"unknown tool: {name}")
            result = {"content": content(TOOLS[name]["handler"](args))}
        else:
            raise ValueError(f"unsupported method: {method}")

        return {"jsonrpc": "2.0", "id": request_id, "result": result}
    except Exception as exc:
        return {
            "jsonrpc": "2.0",
            "id": request_id,
            "error": {"code": -32603, "message": str(exc)},
        }


def main() -> int:
    for line in sys.stdin:
        if not line.strip():
            continue

        response = handle(json.loads(line))
        if response is not None:
            sys.stdout.write(json.dumps(response, separators=(",", ":")) + "\n")
            sys.stdout.flush()

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
