"""Automated smoke tests for Shopping_Portal (piped stdin)."""
from __future__ import annotations

import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXE = ROOT / "build" / "Shopping_Portal.exe"
DATA_FILES = [
    "customer_account_save.txt",
    "vendor_accounts.txt",
    "vendor_products.txt",
    "admin_accounts.txt",
    "orders.txt",
    "ItemsCategory.txt",
    "Groceries.txt",
    "Electronics.txt",
]


def backup_data(tmpdir: Path) -> None:
    for name in DATA_FILES:
        src = ROOT / name
        if src.exists():
            shutil.copy2(src, tmpdir / name)
    # all category product files
    cats = (ROOT / "ItemsCategory.txt").read_text(encoding="utf-8", errors="replace").splitlines()
    for c in cats:
        c = c.strip()
        if not c:
            continue
        src = ROOT / f"{c}.txt"
        if src.exists():
            shutil.copy2(src, tmpdir / f"{c}.txt")


def restore_data(tmpdir: Path) -> None:
    for p in tmpdir.iterdir():
        if p.is_file():
            shutil.copy2(p, ROOT / p.name)


def grocery_stock(name: str = "Apples") -> int | None:
    path = ROOT / "Groceries.txt"
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        m = re.match(rf"^{re.escape(name)}\s*-\s*.+,\s*(\d+)\s*$", line, re.I)
        if m:
            return int(m.group(1))
    return None


def customer_usernames() -> list[str]:
    lines = (ROOT / "customer_account_save.txt").read_text(encoding="utf-8", errors="replace").splitlines()
    users = []
    i = 0
    while i < len(lines):
        u = lines[i].strip()
        if not u:
            i += 1
            continue
        users.append(u)
        i += 2  # skip password
    return users


def run_portal(stdin_text: str, timeout: float = 20.0) -> subprocess.CompletedProcess[str]:
    # Force non-console stdin so password reads use getline
    env = os.environ.copy()
    return subprocess.run(
        [str(EXE)],
        input=stdin_text,
        text=True,
        capture_output=True,
        cwd=str(ROOT),
        timeout=timeout,
        env=env,
    )


def nl(lines: list[str]) -> str:
    return "\n".join(lines) + "\n"


def main() -> int:
    if not EXE.exists():
        print("FAIL: missing build/Shopping_Portal.exe — run build.bat first")
        return 1

    results: list[tuple[str, bool, str]] = []
    tmp = Path(tempfile.mkdtemp(prefix="shop_smoke_"))
    backup_data(tmp)

    try:
        # --- Test 1: duplicate customer registration rejected ---
        user = "smokereg1"
        # purge if leftover
        raw = (ROOT / "customer_account_save.txt").read_text(encoding="utf-8", errors="replace")
        if user in raw:
            # restore clean accounts from backup first
            restore_data(tmp)

        before_users = customer_usernames()
        proc = run_portal(
            nl(
                [
                    "3",  # Customer
                    "1",  # Register
                    user,
                    "pass1234",
                    "pass1234",
                    "",  # pause
                    "1",  # Register again
                    user,
                    "pass1234",
                    "pass1234",
                    "",  # pause after duplicate reject
                    "4",  # back
                    "4",  # exit
                ]
            )
        )
        after_users = customer_usernames()
        count = after_users.count(user)
        out = (proc.stdout or "") + (proc.stderr or "")
        ok = count == 1 and "Already Registered" in out
        results.append(
            (
                "duplicate customer registration rejected",
                ok,
                f"count={count}, returncode={proc.returncode}, has_msg={('Already Registered' in out)}",
            )
        )

        # --- Test 2: logout restocks cart deductions ---
        restore_data(tmp)
        stock0 = grocery_stock("Apples")
        assert stock0 is not None and stock0 >= 1, "Apples stock unavailable for test"
        proc = run_portal(
            nl(
                [
                    "3",
                    "2",  # login
                    "rohaan",
                    "123456",
                    "1",  # browse
                    "1",  # Groceries
                    "1",  # Apples
                    "1",  # qty
                    "0",  # back to categories
                    "0",  # leave browse
                    "9",  # logout (should restock)
                    "4",  # back
                    "4",  # exit
                ]
            )
        )
        stock1 = grocery_stock("Apples")
        ok = stock1 == stock0
        results.append(
            (
                "logout restores deducted stock",
                ok,
                f"before={stock0} after={stock1}",
            )
        )

        # --- Test 3: cart merges same name+price (no duplicate lines) ---
        restore_data(tmp)
        stock0 = grocery_stock("Apples")
        proc = run_portal(
            nl(
                [
                    "3",
                    "2",
                    "rohaan",
                    "123456",
                    "1",
                    "1",
                    "1",
                    "1",
                    "1",  # add Apples again
                    "1",
                    "0",
                    "0",
                    "3",  # display cart
                    "0",
                    "9",
                    "4",
                    "4",
                ]
            )
        )
        out = (proc.stdout or "") + (proc.stderr or "")
        # Cart display should show one Apples line with qty 2
        apples_lines = re.findall(r"Apples\s+Rs\.", out)
        # Also accept table formatting without exact Rs. adjacent
        if not apples_lines:
            apples_lines = re.findall(r"\bApples\b", out)
        # Count occurrences in DISPLAYING ITEMS section if present
        display = out
        idx = display.find("DISPLAYING ITEMS IN CART")
        if idx >= 0:
            chunk = display[idx : idx + 800]
            apples_in_cart = len(re.findall(r"\bApples\b", chunk))
        else:
            apples_in_cart = -1
        stock1 = grocery_stock("Apples")
        ok = apples_in_cart == 1 and stock1 == stock0  # restocked on logout
        results.append(
            (
                "cart merges same item (no duplicate rows) + restock",
                ok,
                f"apples_in_cart_display={apples_in_cart}, stock {stock0}->{stock1}",
            )
        )

        # --- Test 4: addProduct duplicate rejected (via unit-ish file check after vendor path) ---
        restore_data(tmp)
        # Append a duplicate line intentionally then search should dedupe
        g = ROOT / "Groceries.txt"
        text = g.read_text(encoding="utf-8", errors="replace")
        g.write_text(text.rstrip() + "\nApples - 150 PKR, 99\n", encoding="utf-8")
        proc = run_portal(
            nl(
                [
                    "3",
                    "2",
                    "rohaan",
                    "123456",
                    "4",  # search
                    "Apples",
                    "0",
                    "9",
                    "4",
                    "4",
                ]
            )
        )
        out = (proc.stdout or "") + (proc.stderr or "")
        # In SEARCH PRODUCTS section, count Groceries Apples lines
        m = re.search(r"SEARCH PRODUCTS.*?SEARCH IN CART", out, re.S)
        section = m.group(0) if m else out
        hits = re.findall(r"\[Groceries\].*Apples", section)
        ok = len(hits) == 1
        results.append(
            (
                "search dedupes identical catalog rows",
                ok,
                f"hits={len(hits)}",
            )
        )

    except subprocess.TimeoutExpired:
        results.append(("portal run", False, "TIMEOUT"))
    finally:
        restore_data(tmp)
        shutil.rmtree(tmp, ignore_errors=True)

    print("=== Shopping_Portal smoke results ===")
    failed = 0
    for name, ok, detail in results:
        status = "PASS" if ok else "FAIL"
        if not ok:
            failed += 1
        print(f"[{status}] {name} ({detail})")

    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
