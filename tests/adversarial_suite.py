#!/usr/bin/env python3
from __future__ import annotations

import os
import subprocess
import tempfile
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BIN = ROOT / "vskc"


def run(source: str, timeout: float = 5.0):
    with tempfile.NamedTemporaryFile("w", suffix=".vsk", encoding="utf-8", delete=False) as f:
        f.write(source)
        path = f.name
    started = time.perf_counter()
    try:
        p = subprocess.run([str(BIN), path], text=True, errors="replace", capture_output=True, timeout=timeout)
        return p.returncode, p.stdout, p.stderr, (time.perf_counter() - started) * 1000
    except subprocess.TimeoutExpired:
        return "TIMEOUT", "", "", timeout * 1000
    finally:
        os.unlink(path)


def check(name, source, *, code=0, out=None, err=None, timeout=5.0):
    actual_code, stdout, stderr, duration = run(source, timeout)
    passed = actual_code == code
    if out is not None:
        passed = passed and stdout == out
    if err is not None:
        passed = passed and err in stderr
    return {"name": name, "passed": passed, "code": actual_code, "duration_ms": round(duration, 2), "stdout": stdout, "stderr": stderr}


def main():
    cases = [
        check("infinite_repeat_guard", "repeat true {}\n", err="100000 iterations", timeout=5),
        check("deep_nesting", "when true { " * 100 + "say 1" + " }" * 100 + "\n", out="1\n"),
        check("negative_and_decimal_numbers", "say -2.5 + 1.5\n", out="-1\n"),
        check("all_native_modules", 'bring Math\nbring Text\nbring Random\nsay Math.abs(-4)\nsay Text.upper("ok")\n', out="4\nOK\n"),
        check("unsafe_shell_text_is_plain_text", 'say "$(touch /tmp/vsk_should_not_exist)"\n', out="$(touch /tmp/vsk_should_not_exist)\n"),
        check("utf8_string", 'say "مرحبا VSK"\n', out="مرحبا VSK\n"),
        check("utf8_identifier_rejected_currently", 'set متغير = 1\nsay متغير\n', code=1, err="unknown character"),
        check("missing_file_cli", "", code=127),
    ]
    # The missing-file case is executed separately because it has no source file.
    p = subprocess.run([str(BIN), "/tmp/vsk-file-that-does-not-exist"], text=True, errors="replace", capture_output=True)
    cases[-1] = {"name": "missing_file_cli", "passed": p.returncode != 0 and "No such file" in p.stderr, "code": p.returncode, "duration_ms": 0, "stdout": p.stdout, "stderr": p.stderr}
    passed = sum(item["passed"] for item in cases)
    print(f"ADVERSARIAL total={len(cases)} passed={passed} failed={len(cases)-passed}")
    for item in cases:
        print(("PASS" if item["passed"] else "FAIL") + " " + item["name"] + f" ({item['duration_ms']:.2f} ms)")
        if not item["passed"]:
            print(f"  code={item['code']} stdout={item['stdout']!r} stderr={item['stderr']!r}")
    return 0 if passed == len(cases) else 1


if __name__ == "__main__":
    raise SystemExit(main())
