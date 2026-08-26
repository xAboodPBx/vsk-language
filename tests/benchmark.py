from __future__ import annotations

import resource
import subprocess
import time
from pathlib import Path

root = Path(__file__).resolve().parents[1]
started = time.perf_counter()
proc = subprocess.run([str(root / "vskc"), str(root / "tests" / "benchmark.vsk")], text=True, capture_output=True)
elapsed = time.perf_counter() - started
rss_kb = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss
print(f"returncode={proc.returncode}")
print(f"stdout={proc.stdout!r}")
print(f"stderr={proc.stderr!r}")
print(f"elapsed_ms={elapsed * 1000:.2f}")
print(f"child_max_rss_kb={rss_kb}")
raise SystemExit(proc.returncode)
