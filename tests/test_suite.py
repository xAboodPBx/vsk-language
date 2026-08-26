#!/usr/bin/env python3
"""Black-box production-readiness tests for the VSK 0.2 native runtime."""
from __future__ import annotations

import json
import os
import subprocess
import tempfile
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BIN = ROOT / "vskc"

CASES = [
    {
        "name": "empty_program",
        "source": "",
        "stdout": "",
        "returncode": 0,
    },
    {
        "name": "assignment_and_case_insensitivity",
        "source": 'set Name = "VSK"\nname = name + " language"\nNAME += "!"\nsay nAmE\n',
        "stdout": "VSK language!\n",
        "returncode": 0,
    },
    {
        "name": "arithmetic_precedence",
        "source": "set x = 2 + 3 * 4\nsay x\nsay (2 + 3) * 4\nsay 10 / 2\n",
        "stdout": "14\n20\n5\n",
        "returncode": 0,
    },
    {
        "name": "booleans_and_logic",
        "source": "say true and false\nsay true or false\nsay not false\n",
        "stdout": "false\ntrue\ntrue\n",
        "returncode": 0,
    },
    {
        "name": "string_operations",
        "source": 'say "A" + 7\nbring Text\nsay Text.upper("mixed")\nsay Text.lower("MIXED")\n',
        "stdout": "A7\nMIXED\nmixed\n",
        "returncode": 0,
    },
    {
        "name": "function_and_return",
        "source": 'proc add(a, b) { give a + b }\nset result = ADD(2, 5)\nsay result\n',
        "stdout": "7\n",
        "returncode": 0,
    },
    {
        "name": "recursive_function",
        "source": 'proc fact(n) { when n <= 1 { give 1 } otherwise { give n * fact(n - 1) } }\nsay fact(6)\n',
        "stdout": "720\n",
        "returncode": 0,
    },
    {
        "name": "when_otherwise",
        "source": 'set x = 4\nwhen x == 4 { say "yes" } otherwise { say "no" }\n',
        "stdout": "yes\n",
        "returncode": 0,
    },
    {
        "name": "repeat_loop",
        "source": 'set i = 0\nrepeat i < 3 { say i i += 1 }\n',
        "stdout": "0\n1\n2\n",
        "returncode": 0,
    },
    {
        "name": "comments",
        "source": '# comment\n// another comment\nset x = 9 # trailing comment\nsay x\n',
        "stdout": "9\n",
        "returncode": 0,
    },
    {
        "name": "math_module",
        "source": "BRING math\nsay MATH.sqrt(144)\nsay math.pi\n",
        "stdout": "12\n3.14159\n",
        "returncode": 0,
    },
    {
        "name": "unused_library_is_ignored",
        "source": 'bring Math\nbring Text\nsay "ok"\n',
        "stdout": "ok\n",
        "stderr_not_contains": "notice",
        "returncode": 0,
    },
    {
        "name": "missing_library_continues",
        "source": 'bring database_driver\nsay "continued"\n',
        "stdout": "continued\n",
        "stderr_contains": "database_driver",
        "returncode": 0,
    },
    {
        "name": "missing_name_continues",
        "source": 'say unknown_value\nsay "continued"\n',
        "stdout": "nil\ncontinued\n",
        "stderr_contains": "unknown_value",
        "returncode": 0,
    },
    {
        "name": "division_by_zero_is_nonfatal",
        "source": "say 1 / 0\nsay 5\n",
        "stdout": "nil\n5\n",
        "stderr_contains": "division by zero",
        "returncode": 0,
    },
    {
        "name": "syntax_unclosed_string",
        "source": 'say "unclosed\n',
        "stdout": "",
        "stderr_contains": "unclosed string",
        "returncode": 1,
    },
    {
        "name": "syntax_unknown_character",
        "source": "say @\n",
        "stdout": "",
        "stderr_contains": "unknown character",
        "returncode": 1,
    },
    {
        "name": "syntax_missing_brace",
        "source": "when true { say 1\n",
        "stdout": "",
        "stderr_contains": "unexpected",
        "returncode": 1,
    },
    {
        "name": "long_string_boundary",
        "source": 'say "' + ('x' * 4095) + '"\n',
        "stdout": ('x' * 4095) + "\n",
        "returncode": 0,
    },
    {
        "name": "long_string_rejected",
        "source": 'say "' + ('x' * 4096) + '"\n',
        "stdout": "",
        "stderr_contains": "string too long",
        "returncode": 1,
    },
]


def run_case(case: dict) -> dict:
    started = time.perf_counter()
    with tempfile.NamedTemporaryFile("w", suffix=".vsk", encoding="utf-8", delete=False) as handle:
        handle.write(case["source"])
        path = handle.name
    try:
        proc = subprocess.run([str(BIN), path], text=True, capture_output=True, timeout=5)
    except subprocess.TimeoutExpired:
        return {"name": case["name"], "passed": False, "reason": "timeout", "duration_ms": 5000}
    finally:
        os.unlink(path)
    checks = []
    checks.append((proc.returncode == case.get("returncode", 0), f"returncode={proc.returncode}"))
    if "stdout" in case:
        checks.append((proc.stdout == case["stdout"], f"stdout={proc.stdout!r}"))
    if "stderr_contains" in case:
        checks.append((case["stderr_contains"] in proc.stderr, f"stderr={proc.stderr!r}"))
    if "stderr_not_contains" in case:
        checks.append((case["stderr_not_contains"] not in proc.stderr, f"stderr={proc.stderr!r}"))
    passed = all(ok for ok, _ in checks)
    return {
        "name": case["name"],
        "passed": passed,
        "duration_ms": round((time.perf_counter() - started) * 1000, 2),
        "details": [] if passed else [detail for ok, detail in checks if not ok],
        "stdout": proc.stdout,
        "stderr": proc.stderr,
        "returncode": proc.returncode,
    }


def main() -> int:
    if not BIN.exists():
        print(json.dumps({"error": f"missing binary: {BIN}"}, indent=2))
        return 2
    results = [run_case(case) for case in CASES]
    passed = sum(item["passed"] for item in results)
    report = {
        "binary": str(BIN),
        "total": len(results),
        "passed": passed,
        "failed": len(results) - passed,
        "pass_rate": round(passed / len(results) * 100, 2),
        "results": results,
    }
    report_path = ROOT / "test-results.json"
    report_path.write_text(json.dumps(report, indent=2), encoding="utf-8")
    print(json.dumps({k: report[k] for k in ("total", "passed", "failed", "pass_rate")}, indent=2))
    for item in results:
        marker = "PASS" if item["passed"] else "FAIL"
        print(f"{marker:4} {item['name']:38} {item['duration_ms']:8.2f} ms")
        for detail in item.get("details", []):
            print(f"      {detail}")
    return 0 if passed == len(results) else 1


if __name__ == "__main__":
    raise SystemExit(main())
