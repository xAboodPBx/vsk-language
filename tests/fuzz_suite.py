from __future__ import annotations

import random
import subprocess
import tempfile
import os
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BIN = ROOT / "vskc"
rng = random.Random(20260827)

atoms = ["0", "1", "2.5", "true", "false", "nil", '"x"', '"hello"', "unknown"]
ops = ["+", "-", "*", "/", "%", "==", "!=", "<", ">", "and", "or"]


def expr(depth=0):
    if depth > 2 or rng.random() < 0.55:
        return rng.choice(atoms)
    return f"({expr(depth + 1)} {rng.choice(ops)} {expr(depth + 1)})"


def program(index):
    lines = []
    for _ in range(rng.randint(1, 12)):
        choice = rng.randrange(9)
        if choice == 0:
            lines.append(f"set x{rng.randrange(5)} = {expr()}")
        elif choice == 1:
            lines.append(f"x{rng.randrange(5)} += {rng.randrange(4)}")
        elif choice == 2:
            lines.append(f"say {expr()}")
        elif choice == 3:
            lines.append(f"bring {rng.choice(['Math', 'Text', 'Random', 'missing_pkg'])}")
        elif choice == 4:
            lines.append(f"when {rng.choice(['true', 'false', 'x0 == x1'])} {{ say {expr()} }}")
        elif choice == 5:
            lines.append(f"repeat false {{ say {expr()} }}")
        elif choice == 6:
            lines.append(f"proc f{index}(a) {{ give {expr()} }}")
        elif choice == 7:
            lines.append(f"say Math.{rng.choice(['sqrt', 'abs', 'pi', 'missing'])}({rng.randrange(10)})")
        else:
            lines.append(rng.choice(["# comment", "// comment", "", "say (1 + 2) * 3"]))
    if index % 10 == 0:
        lines.append(rng.choice(["say @", 'say "unclosed', "when true { say 1", "set = 2"]))
    return "\n".join(lines) + "\n"


def main():
    total = 300
    crashes = 0
    timeouts = 0
    invalid_codes = 0
    samples = []
    for index in range(total):
        source = program(index)
        with tempfile.NamedTemporaryFile("w", suffix=".vsk", encoding="utf-8", delete=False) as f:
            f.write(source)
            path = f.name
        try:
            try:
                p = subprocess.run([str(BIN), path], capture_output=True, timeout=1)
            except subprocess.TimeoutExpired:
                timeouts += 1
                continue
            if p.returncode < 0 or p.returncode > 1:
                invalid_codes += 1
            if p.returncode < 0:
                crashes += 1
                if len(samples) < 3:
                    samples.append((source, p.returncode))
        finally:
            os.unlink(path)
    passed = crashes == 0 and timeouts == 0 and invalid_codes == 0
    print(f"FUZZ total={total} crashes={crashes} timeouts={timeouts} invalid_codes={invalid_codes}")
    if samples:
        for source, code in samples:
            print(f"sample crash code={code}: {source!r}")
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
