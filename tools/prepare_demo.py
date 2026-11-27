"""Copy the maintained seed into an isolated runtime; never overwrite by default."""

from pathlib import Path
import argparse, shutil

root = Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser()
p.add_argument("--reset", action="store_true")
a = p.parse_args()
target = root / "data/runtime"
if target.exists() and not a.reset:
    print("Runtime retained:", target)
else:
    target.mkdir(parents=True, exist_ok=True)
    for item in (root / "data/sample").glob("*.txt"):
        shutil.copy2(item, target / item.name)
    print("Demo data prepared:", target)
