#!/usr/bin/env python3

import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path


class NotaryzeCliFlowTest(unittest.TestCase):
    def setUp(self):
        if sys.platform != "darwin":
            self.skipTest("codesign flow tests require macOS")
        if len(sys.argv) < 2:
            self.fail("Notaryze executable path argument is required")
        if shutil.which("cc") is None:
            self.skipTest("C compiler is required to create a sample executable")
        if shutil.which("codesign") is None:
            self.skipTest("codesign is required")

        self.notaryze = Path(sys.argv[1]).resolve()
        self.assertTrue(self.notaryze.exists(), self.notaryze)

    def run_cli(self, *args, check=False):
        result = subprocess.run(
            [str(self.notaryze), *map(str, args)],
            text=True,
            capture_output=True,
            check=False,
        )

        if check and result.returncode != 0:
            self.fail(
                f"command failed: {args}\n"
                f"exit={result.returncode}\nstdout={result.stdout}\nstderr={result.stderr}"
            )

        return result

    def create_sample_executable(self, directory: Path) -> Path:
        source = directory / "hello.c"
        binary = directory / "hello"
        source.write_text(
            '#include <stdio.h>\nint main(void) { puts("hello"); return 0; }\n',
            encoding="utf-8",
        )
        subprocess.run(["cc", str(source), "-o", str(binary)], check=True)
        return binary

    def test_help_is_available(self):
        result = self.run_cli("--help", check=True)
        self.assertIn("--status <path>", result.stdout)
        self.assertIn("--adhoc-sign <path>", result.stdout)

    def test_unsigned_to_signed_flow(self):
        with tempfile.TemporaryDirectory(prefix="notaryze-cli-test.") as tmp:
            binary = self.create_sample_executable(Path(tmp))

            self.run_cli("--remove-signature", binary, check=True)

            status = self.run_cli("--status", binary, check=True)
            self.assertEqual(status.stdout.strip(), "Unsigned")

            verify_unsigned = self.run_cli("--verify", binary)
            self.assertNotEqual(verify_unsigned.returncode, 0)
            self.assertIn("not signed", verify_unsigned.stdout + verify_unsigned.stderr)

            self.run_cli("--adhoc-sign", binary, check=True)

            status = self.run_cli("--status", binary, check=True)
            self.assertEqual(status.stdout.strip(), "Signed")

            verify_signed = self.run_cli("--verify", binary, check=True)
            self.assertIn("valid on disk", verify_signed.stdout + verify_signed.stderr)


if __name__ == "__main__":
    unittest.main(argv=[sys.argv[0]])
