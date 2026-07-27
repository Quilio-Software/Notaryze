#!/usr/bin/env python3

import json
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path


class NotaryzeMcpFlowTest(unittest.TestCase):
    def setUp(self):
        if sys.platform != "darwin":
            self.skipTest("codesign flow tests require macOS")
        if len(sys.argv) < 2:
            self.fail("MCP server path argument is required")
        if shutil.which("codesign") is None:
            self.skipTest("codesign is required")
        if shutil.which("cc") is None:
            self.skipTest("C compiler is required")

        self.server = Path(sys.argv[1]).resolve()
        self.assertTrue(self.server.exists(), self.server)

    def call_server(self, requests):
        payload = "\n".join(json.dumps(request) for request in requests) + "\n"
        result = subprocess.run(
            [sys.executable, str(self.server)],
            input=payload,
            text=True,
            capture_output=True,
            check=False,
        )
        self.assertEqual(result.returncode, 0, result.stderr)
        return [json.loads(line) for line in result.stdout.splitlines() if line.strip()]

    def tool_payload(self, response):
        text = response["result"]["content"][0]["text"]
        return json.loads(text)

    def test_unsigned_to_signed_flow(self):
        with tempfile.TemporaryDirectory(prefix="notaryze-mcp-test.") as tmp:
            requests = [
                {"jsonrpc": "2.0", "id": 1, "method": "initialize", "params": {}},
                {"jsonrpc": "2.0", "id": 2, "method": "tools/list", "params": {}},
                {
                    "jsonrpc": "2.0",
                    "id": 3,
                    "method": "tools/call",
                    "params": {
                        "name": "create_sample_executable",
                        "arguments": {"output_dir": tmp},
                    },
                },
                {
                    "jsonrpc": "2.0",
                    "id": 4,
                    "method": "tools/call",
                    "params": {
                        "name": "remove_signature",
                        "arguments": {"path": str(Path(tmp) / "hello")},
                    },
                },
                {
                    "jsonrpc": "2.0",
                    "id": 5,
                    "method": "tools/call",
                    "params": {
                        "name": "verify_signature",
                        "arguments": {"path": str(Path(tmp) / "hello")},
                    },
                },
                {
                    "jsonrpc": "2.0",
                    "id": 6,
                    "method": "tools/call",
                    "params": {
                        "name": "sign_ad_hoc",
                        "arguments": {"path": str(Path(tmp) / "hello")},
                    },
                },
                {
                    "jsonrpc": "2.0",
                    "id": 7,
                    "method": "tools/call",
                    "params": {
                        "name": "verify_signature",
                        "arguments": {"path": str(Path(tmp) / "hello")},
                    },
                },
            ]

            responses = self.call_server(requests)
            tools = {tool["name"] for tool in responses[1]["result"]["tools"]}
            self.assertIn("verify_signature", tools)
            self.assertIn("remove_signature", tools)
            self.assertIn("sign_ad_hoc", tools)

            created = self.tool_payload(responses[2])
            self.assertEqual(created["compile"]["exit_code"], 0)
            self.assertTrue(Path(created["path"]).exists())

            removed = self.tool_payload(responses[3])
            self.assertEqual(removed["remove"]["exit_code"], 0)
            self.assertFalse(removed["verification"]["signed"])

            unsigned = self.tool_payload(responses[4])
            self.assertFalse(unsigned["signed"])
            self.assertNotEqual(unsigned["verify"]["exit_code"], 0)

            signed = self.tool_payload(responses[5])
            self.assertEqual(signed["sign"]["exit_code"], 0)
            self.assertTrue(signed["verification"]["signed"])

            verified = self.tool_payload(responses[6])
            self.assertTrue(verified["signed"])
            self.assertEqual(verified["verify"]["exit_code"], 0)


if __name__ == "__main__":
    unittest.main(argv=[sys.argv[0]])
