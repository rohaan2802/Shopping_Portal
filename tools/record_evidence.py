"""Record real CLI/native results using temporary fixtures, never maintained seed files.

Usage (after building into ./build):
    python tools/record_evidence.py

Environment overrides for cross-built checks:
    SHOPPING_PORTAL_RUNNER  launcher prefix for Windows executables (for example "wine")
"""

from pathlib import Path
from contextlib import contextmanager
import os, shlex, subprocess, shutil, sys, uuid

ROOT = Path(__file__).resolve().parents[1]
REPORTS = ROOT / "reports"
BUILD = ROOT / "build"
TEST_RUNTIME = ROOT / ".test-runtime"
RUNNER = shlex.split(os.environ.get("SHOPPING_PORTAL_RUNNER", ""))


def binary(name):
    """Find a built target in single- or multi-config CMake layouts."""
    for folder in (BUILD, BUILD / "Release", BUILD / "Debug"):
        for candidate in (folder / name, folder / f"{name}.exe"):
            if candidate.exists():
                return candidate
    raise FileNotFoundError(f"{name} not built; run: cmake --build build")


def windows(name):
    path = binary(name)
    return [*RUNNER, path] if path.suffix == ".exe" else [path]


@contextmanager
def temporary_workspace(prefix):
    """Disposable evidence fixture in an ignored, project-local folder."""
    TEST_RUNTIME.mkdir(exist_ok=True)
    path = TEST_RUNTIME / f"{prefix}{uuid.uuid4().hex}"
    shutil.copytree(ROOT / "data/sample", path)
    try:
        yield path
    finally:
        shutil.rmtree(path, ignore_errors=True)
        try:
            TEST_RUNTIME.rmdir()
        except OSError:
            pass


def run(args, cwd=ROOT, stdin=None):
    p = subprocess.run(
        [str(x) for x in args],
        cwd=cwd,
        input=stdin,
        text=True,
        capture_output=True,
        timeout=120,
    )
    if p.returncode:
        raise RuntimeError(p.stdout + p.stderr)
    return p.stdout + p.stderr


def save(name, text):
    (REPORTS / name).write_text(text, encoding="utf-8", newline="\n")


CLI_REPORTS = [
    ("catalog.json", ["--json"]),
    ("summary.txt", ["--summary"]),
    ("search.txt", ["--search", "oil"]),
    ("low_stock.txt", ["--low-stock", "--sort", "stock"]),
    ("price_sort.txt", ["--category", "Electronics", "--sort", "price"]),
    ("price_stats.txt", ["--stats"]),
    ("restock_plan.txt", ["--restock", "8", "--category", "Electronics"]),
    ("quote.txt", ["--quote", "Apples:3,Milk:2,Bread:1", "--discount", "10"]),
    ("top_value.csv", ["--sort", "value", "--desc", "--limit", "12", "--csv"]),
]

SCENARIOS = {
    "welcome": ["4"],
    "browse": ["3", "2", "rohaan", "123456", "1", "1", "0", "0", "9", "4", "4"],
    "checkout": ["3", "2", "rohaan", "123456", "1", "1", "1", "2", "0", "0",
                 "6", "1", "", "8", "", "9", "4", "4"],
    "wishlist": ["3", "2", "rohaan", "123456", "7", "1", "Apples", "", "7", "1", "Headphones", "",
                 "7", "3", "", "9", "4", "4"],
    "admin": ["1", "2", "admin1", "admin12", "6", "4"],
    "insights": ["1", "2", "admin1", "admin12", "5", "", "6", "4"],
    "vendor": ["2", "2", "vendor1", "vendor1", "1", "1", "", "5", "4", "4"],
    "cart": ["3", "2", "rohaan", "123456", "1", "1", "1", "2", "1", "1", "4", "1", "0", "0",
             "3", "0", "9", "4", "4"],
}


def main():
    REPORTS.mkdir(exist_ok=True)
    cli = binary("catalog_cli")
    for name, args in CLI_REPORTS:
        save(name, run([cli, "--data", ROOT / "data/sample", *args]))
    save("domain_tests.txt", run([binary("domain_tests")]))
    save("analytics_tests.txt", run([binary("analytics_tests"), ROOT / "data/sample"]))
    save("ctest.txt", run(["ctest", "--test-dir", BUILD, "-C", "Release", "--output-on-failure"]))
    save("smoke_tests.txt", run([sys.executable, ROOT / "tests/native_smoke.py"]))
    with temporary_workspace("shop_console_") as d:
        save("console_tests.txt", run(windows("console_tests"), d))
    for name, lines in SCENARIOS.items():
        with temporary_workspace("shop_evidence_") as d:
            output = run(windows("Shopping_Portal"), d, "\n".join(lines) + "\n")
            save(f"native_{name}.txt", output)
            if name == "checkout":
                assert "ORDER PLACED - ID #" in output, "Checkout evidence did not place an order"
                save("checkout_orders.txt", (d / "orders.txt").read_text())
                save("checkout_stock.txt", (d / "Groceries.txt").read_text())
    sys.path.insert(0, str(ROOT / "tools"))
    sys.dont_write_bytecode = True  # no tools/__pycache__ in the repo
    import build_site_data  # refresh assets/js/data.js for the webpage

    build_site_data.main()
    print("Evidence refreshed from real isolated runs.")


if __name__ == "__main__":
    main()
