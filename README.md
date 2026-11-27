<div align="center">

<img src="assets/favicon.svg" alt="Shopping_Portal logo" width="88">

# Shopping_Portal

**A C++17 object-oriented shopping system with Admin, Vendor and Customer roles, exact PKR billing, rollback-safe checkout, inventory analytics and an interactive browser demo.**

<a href="https://en.cppreference.com/w/cpp/17"><img src="https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white" alt="C%2B%2B: 17" /></a>
<a href="CMakeLists.txt"><img src="https://img.shields.io/badge/Build-CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white" alt="Build: CMake" /></a>
<a href="Shopping_Portal.sln"><img src="https://img.shields.io/badge/IDE-Visual%20Studio-5C2D91?style=for-the-badge&logo=visualstudio&logoColor=white" alt="IDE: Visual Studio" /></a>
<a href="src/Main.cpp"><img src="https://img.shields.io/badge/UI-Windows%20Console-0078D6?style=for-the-badge&logo=windows&logoColor=white" alt="UI: Windows Console" /></a>
<a href="src/catalog_cli.cpp"><img src="https://img.shields.io/badge/CLI-Linux%20%7C%20macOS%20%7C%20Windows-FCC624?style=for-the-badge&logo=linux&logoColor=white" alt="CLI: Linux | macOS | Windows" /></a>
<a href="reports/ctest.txt"><img src="https://img.shields.io/badge/Tests-191%20passing-2EA043?style=for-the-badge&logo=pytest&logoColor=white" alt="Tests: 191 passing" /></a>
<a href="CMakeLists.txt"><img src="https://img.shields.io/badge/CTest-7%20targets-064F8C?style=for-the-badge&logo=cmake&logoColor=white" alt="CTest: 7 targets" /></a>
<a href="include/Analytics.h"><img src="https://img.shields.io/badge/Analytics-Stats%20%7C%20Restock%20%7C%20Quotes-7C3AED?style=for-the-badge&logo=googleanalytics&logoColor=white" alt="Analytics: Stats | Restock | Quotes" /></a>
<a href="index.html"><img src="https://img.shields.io/badge/Web-HTML5-E34F26?style=for-the-badge&logo=html5&logoColor=white" alt="Web: HTML5" /></a>
<a href="assets/css/site.css"><img src="https://img.shields.io/badge/Style-CSS3-1572B6?style=for-the-badge&logo=css&logoColor=white" alt="Style: CSS3" /></a>
<a href="assets/js/site.js"><img src="https://img.shields.io/badge/Demo-JavaScript-F7DF1E?style=for-the-badge&logo=javascript&logoColor=white" alt="Demo: JavaScript" /></a>
<a href="tests/native_smoke.py"><img src="https://img.shields.io/badge/Harness-Python-3776AB?style=for-the-badge&logo=python&logoColor=white" alt="Harness: Python" /></a>
<a href="tests/site_check.py"><img src="https://img.shields.io/badge/Browser%20tests-Playwright-2EAD33?style=for-the-badge&logo=playwright&logoColor=white" alt="Browser tests: Playwright" /></a>
<a href=".clang-format"><img src="https://img.shields.io/badge/Code%20style-clang--format-262D3A?style=for-the-badge&logo=llvm&logoColor=white" alt="Code style: clang--format" /></a>
<a href="reports/catalog.json"><img src="https://img.shields.io/badge/Data-351%20products-0F766E?style=for-the-badge&logo=databricks&logoColor=white" alt="Data: 351 products" /></a>
<a href="https://rohaan2802.github.io/Shopping_Portal/"><img src="https://img.shields.io/badge/Pages-Live%20demo-222222?style=for-the-badge&logo=githubpages&logoColor=white" alt="Pages: Live demo" /></a>
<a href="https://github.com/rohaan2802"><img src="https://img.shields.io/badge/Author-Rohaan-181717?style=for-the-badge&logo=github&logoColor=white" alt="Author: Rohaan" /></a>

**[🌐 Live webpage](https://rohaan2802.github.io/Shopping_Portal/)** · **[📦 Repository](https://github.com/rohaan2802/Shopping_Portal)** · **[🖼️ Screenshots](#-screenshots)** · **[🚀 Quick start](#-setup)**

</div>

---

## 📑 Contents

1. [Overview](#-overview)
2. [Features](#-features)
3. [What's new in this version](#-whats-new-in-this-version)
4. [Tech stack](#-tech-stack)
5. [Architecture](#-architecture)
6. [Data flow](#-data-flow)
7. [Classes and data structures](#-classes-and-data-structures)
8. [Key algorithms](#-key-algorithms)
9. [Design decisions](#-design-decisions)
10. [Folder structure](#-folder-structure)
11. [Setup](#-setup)
12. [Commands reference](#-commands-reference)
13. [Demo walkthrough](#-demo-walkthrough)
14. [Feature walkthroughs](#-feature-walkthroughs)
15. [catalog_cli reference](#-catalog_cli-reference)
16. [Billing and data rules](#-billing-and-data-rules)
17. [Testing and results](#-testing-and-results)
18. [Webpage](#-webpage)
19. [Screenshots](#-screenshots)
20. [Limitations](#-limitations)
21. [About the author](#-about-the-author)

## 🧭 Overview

Shopping_Portal began as an object-oriented programming course project and has been extended into a tested, documented C++17 system. It models a small online store backed by plain text files:

- **Admin** manages users, categories and products, reviews orders with exact revenue and average order value, and opens an **Inventory Insights** dashboard.
- **Vendor** registers a company and manages only the products it owns.
- **Customer** browses 12 categories, searches, builds a cart that merges matching lines, keeps a saved wishlist, checks out and reviews order history.

The sample catalog holds **351 products in 12 categories (4,651 stock units, PKR 5,975,360.00 of inventory)**. Three entry points share one domain layer: the Windows console portal, a portable read-only audit CLI (`catalog_cli`) and a responsive webpage with a browser-only shopping sandbox.

## ✨ Features

| | Feature | Where |
| --- | --- | --- |
| 👤 | Abstract `Role` contract with Admin, Vendor and Customer implementations | `include/Role.h`, `src/*.cpp` |
| 🛒 | Cart merging, quantity edits, removal, persistent wishlist, order history | `src/Cart.cpp`, `src/Customer.cpp` |
| 💰 | Integer-paisa money; 5% tax and 5% delivery, each rounded half up | `include/Commerce.h` |
| 🛡️ | Whole-cart validation, staged multi-file commit, automatic rollback | `include/Commerce.h` |
| 📊 | Admin Inventory Insights: category medians and values, low-stock alerts, restock estimate | `src/Admin.cpp` |
| 📈 | Price statistics (min / median / mean / max) and restock planning | `include/Analytics.h` |
| 🧾 | Stock-checked basket quotes with up to 50% discount before tax | `catalog_cli --quote` |
| 🔎 | Search, category, price-range and low-stock filters; value sorting; limits | `catalog_cli` |
| 📤 | JSON and CSV export | `catalog_cli --json / --csv` |
| 🌐 | Webpage with live inventory insights, shopping sandbox, discount codes and screenshot viewer | `index.html` |
| ✅ | 191 automated checks across five suites, including headless browser tests over `file://` and `http://` | `tests/` |

## 🆕 What's new in this version

- **`include/Analytics.h`** – a new standard-library-only analytics module: `priceStatistics`, `Filter`/`select`, `sortProducts` (name, price, stock or stock value, ascending or descending), `restockPlan`, `buildQuote` with discounts, `toCsv` and grouped money formatting.
- **Admin → 5) Inventory Insights** – a new read-only native report built on the analytics module. **Logout moved to option 6.**
- **Exact admin revenue** – order totals are summed in integer paisa (previously `double` truncated to an `int`), and the report now shows the average order value.
- **`catalog_cli` v2** – new options `--min-price`, `--max-price`, `--threshold`, `--desc`, `--limit`, `--sort value`, `--stats`, `--categories`, `--restock`, `--quote`, `--discount` and `--csv`, with a full `--help` page and clear errors (exit code 2).
- **Tests** – a 30-check `analytics_tests` suite, four new CTest entries for the CLI, a ninth integration scenario for the admin report, and `tests/site_check.py` (99 browser checks, run over both `file://` and `http://`).
- **Tooling** – `record_evidence.py` finds single- or multi-config build outputs and records nine CLI reports and eight native transcripts; `render_figures.py` produces all 30 screenshots, including real Playwright browser captures.
- **Webpage redesign** – right-aligned navbar with 12 sections, icon theme toggle, visible mobile menu icon, a new back-to-top button with a scroll-progress ring, a new light and dark palette, larger 18 px body text, live insights, price statistics and discount codes in the sandbox.
- **Works offline** – the page no longer uses `fetch()`. `tools/build_site_data.py` bundles the catalog and screenshot list into `assets/js/data.js` and writes the 30 screenshots into `index.html` as plain `<img>` tags, so double-clicking `index.html` works the same as GitHub Pages.
- **Deeper documentation** – modules, classes, data flow, step-by-step feature walkthroughs, algorithms with complexity, a test-coverage matrix, design decisions and a command reference, on the webpage and in this README.

## 🧰 Tech stack

| Layer | Technology |
| --- | --- |
| Language | C++17 (STL, `<filesystem>`, `<regex>`), object-oriented design with inheritance and virtual functions |
| Native UI | Windows Console API (colours, layout, buffered input) |
| Build | CMake 3.20+ with CTest, Visual Studio 2022 solution, MinGW-w64 / GCC |
| Tests | C++ test executables, Python 3 integration harness, Playwright browser checks |
| Web | HTML5, CSS3 (custom properties, light and dark themes), vanilla JavaScript |
| Tooling | Python + Pillow screenshot renderer, `.clang-format`, `.editorconfig`, VS Code tasks |

## 🏗️ Architecture

```text
 Shopping_Portal.exe (Windows)      catalog_cli (portable)        index.html (browser)
  Admin · Vendor · Customer          read-only audit + export      sandbox + insights
            │                                 │                            │
            └──────────────┬──────────────────┘                            │
                           ▼                                               │
                 include/Analytics.h  ── statistics · filters · restock · quotes · CSV
                 include/Commerce.h   ── parsing · integer money · rollback commits
                           │                                               │
                           ▼                                               ▼
        data/sample  ──copy──►  data/runtime (ignored)        reports/catalog.json (export)
```

| Component | Files | Responsibility |
| --- | --- | --- |
| Role contract | `include/Role.h`, `Admin.h`, `Vendor.h`, `Customer.h` | Abstract registration/login and role dashboards |
| Entry and console UI | `src/Main.cpp`, `include/Other_Fun.h` | Role selection, console rendering, validated input |
| Inventory | `src/Items.cpp`, `include/Items.h` | Category registry, product CRUD and search |
| Cart and billing | `src/Cart.cpp`, `include/Cart.h` | Merging, quantities, bills and stock checks |
| Domain core | `include/Commerce.h` | Complete-record parsing, paisa arithmetic, staged commits |
| Analytics | `include/Analytics.h` | Statistics, filters, sorting, restock plans, quotes, CSV |
| Audit CLI | `src/catalog_cli.cpp` | Command-line access to every analytics feature |
| Webpage | `index.html`, `assets/css/site.css`, `assets/js/site.js` | Documentation, insights, sandbox, screenshot viewer |
| Tooling | `tools/` | Demo reset, evidence recording, screenshot rendering, webpage data bundle |

More detail: [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

### Modules

| Module | Lines | Key functions |
| --- | ---: | --- |
| `src/Main.cpp` | 99 | `main()`: console set-up and the role selection loop |
| `src/Admin.cpp` | 529 | `admin_menu()`, `viewOrdersAndStats()`, `viewInventoryInsights()` |
| `src/Vendor.cpp` | 414 | `vendor_menu()`, own-product view, add, update and remove |
| `src/Customer.cpp` | 615 | `show_customer_menu()`, `checkoutAndSaveOrder()`, wishlist, order history, password change |
| `src/Items.cpp` | 968 | `print_Items_Menu()`, `addProduct()`, `updateProduct()`, `removeCategory()`, `searchAllCatalogs()` |
| `src/Cart.cpp` | 386 | `add_item()`, `Bill()`, `validateAndDeductStock()` |
| `include/Commerce.h` | 241 | `money()`, `calculate()`, `parseProduct()`, `loadCatalog()`, `commitFiles()` |
| `include/Analytics.h` | 297 | `priceStatistics()`, `select()`, `sortProducts()`, `restockPlan()`, `buildQuote()`, `toCsv()` |
| `include/Other_Fun.h` | 789 | Console colours, centred layout, masked passwords, range-checked input |
| `src/catalog_cli.cpp` | 346 | Option parsing and every CLI report |

## 🔄 Data flow

1. **Seed** – `data/sample/` holds `ItemsCategory.txt` (the category index), one file per category (`Name - price PKR, quantity`), the demo accounts, sample orders and wishlists. It is versioned and never edited by the app.
2. **Runtime copy** – `run.bat` calls `tools/prepare_demo.py`, which copies the seed into the git-ignored `data/runtime/`. The portal runs inside that folder, so stock, orders and wishlists change only locally.
3. **Load and validate** – `shop::loadCatalog()` reads the index, rejects unsafe or duplicate category names (`../`, `CON`, `NUL`…), parses each record and rejects malformed rows, duplicates, bad prices and negative stock.
4. **Transact** – at checkout `Cart::validateAndDeductStock()` re-reads stock for every cart line, builds the new file contents in memory and hands them, with the new order record, to `shop::commitFiles()`.
5. **Audit** – `catalog_cli` loads the same files read-only and prints summaries, statistics, restock plans, quotes, JSON or CSV.
6. **Evidence and webpage** – `tools/record_evidence.py` runs the CLI and the real executable in throw-away copies of the seed and saves `reports/`. `tools/render_figures.py` turns those reports into the 30 screenshots, and `tools/build_site_data.py` bundles `reports/catalog.json` and the screenshot list into `assets/js/data.js` for the webpage.

## 🧱 Classes and data structures

```text
            ┌──────────────────────┐
            │ «abstract» Role      │  reg_name, reg_password, login_name …
            │ + registraion() = 0  │
            │ + login() = 0        │
            └──────────┬───────────┘
        public ┌───────┼────────┐ protected
       ┌───────┴──┐ ┌──┴─────┐ ┌┴──────────┐
       │  Admin   │ │ Vendor │ │ Customer  │──uses──▶ Cart ──reads/writes──▶ category files
       └────┬─────┘ └───┬────┘ └─────┬─────┘           │
            └───────────┴──── uses ──┴──▶ Item         └──▶ shop::commitFiles()
                                   (catalog CRUD, search)
```

| Type | Kind | Role in the system |
| --- | --- | --- |
| `Role` | Abstract base class | Registration and login fields; pure virtual `registraion()` and `login()`, virtual destructor |
| `Admin : public Role` | Class | Single admin account; users, categories, products, orders & stats, **Inventory Insights**; revenue summed in paisa |
| `Vendor : public Role` | Class | Company name; `vendor_products.txt` maps products to vendors so a vendor edits only its own listings |
| `Customer : protected Role` | Class | Protected inheritance hides the base interface; current user, 100-slot wishlist, browsing, checkout, history |
| `Cart` | Class | Fixed 500-line arrays of names, prices and quantities; merges identical name + price lines; checkout validation and stock deduction |
| `Item` | Class (mostly static) | Category registry, product CRUD (blocked on unknown categories), category removal only when empty, de-duplicated search |
| `shop::Product`, `shop::Totals` | Structs | Category, name, price label, price in paisa, quantity; subtotal, tax, delivery, total (`using Cents = std::int64_t`) |
| `analytics::PriceStats`, `Filter`, `RestockPlan`, `Quote` | Structs | Result types returned by the analytics functions, easy to print, test and export |

## 🧠 Key algorithms

*n* = number of products (351 in the sample), *k* = cart or quote lines.

| # | Algorithm | How it works | Complexity |
| --- | --- | --- | --- |
| 1 | Complete-record parsing | Find the **last** `" - "` and the **last** comma so names like `T-shirts` survive; validate name, price (≤ 2 decimals) and a 1–9 digit quantity | O(line length) |
| 2 | Exact 5% charges | `charge = (s / 10000) * bp + ((s % 10000) * bp + 5000) / 10000` with `bp = 500`; the quotient/remainder split avoids overflow | O(1) |
| 3 | Rollback-capable commit | Write every `.shop.tmp`; rename originals to `.shop.bak` and temps into place; delete backups only after all succeed; on any error restore backups and remove temps. Leftover `.tmp`/`.bak` files block the next commit | O(bytes written) |
| 4 | Median / mean per category | Group in a `std::map`, sort each group by price, take the middle (or the half-up average of the two middle prices) and the half-up mean | O(n log n) |
| 5 | Restock plan | For each product below the target, order `target − stock` units at catalog price; sort by stock, then name; targets outside 1–100,000 are rejected | O(n log n) |
| 6 | Quote merging | Match each `Name:qty` case-insensitively; more than one match is ambiguous; repeated items add to one line; merged quantity is checked against stock | O(k · n) |
| 7 | Deterministic sorting | `std::stable_sort` by price, stock or value (price × stock), ties broken by lower-case name | O(n log n) |
| 8 | CSV escaping | Fields with a comma, quote or newline are quoted and inner quotes doubled | O(field length) |

## 🧭 Design decisions

- **Integer money.** Binary floating point cannot represent 0.10 exactly; 64-bit paisa keeps every bill exact and the rounding rule explicit.
- **Header-only domain layer.** `Commerce.h` and `Analytics.h` use only the standard library, so the same rules compile into the Windows app, the portable CLI and the unit tests.
- **Stage, back up, then swap.** Text files have no transactions; temp files plus backups let checkout undo a half-finished update.
- **Read-only analytics.** Reports and quotes never write data, so they are safe to run at any time.
- **Seed vs runtime data.** Demos and tests run on copies, so results are repeatable and the versioned seed stays clean.
- **Reject, don't guess.** Ambiguous names, inverted price ranges, unknown sort keys and overselling fail with a clear message instead of picking an arbitrary record.
- **Static webpage, no server.** Data is bundled into `assets/js/data.js` and screenshots are plain `<img>` tags, so the page works from `file://`, any static host and GitHub Pages.

## 📁 Folder structure

```text
Shopping_Portal/
├── src/                     Native portal (Main, Admin, Vendor, Customer, Items, Cart) + catalog_cli.cpp
├── include/                 Headers, including Commerce.h and Analytics.h
├── tests/
│   ├── domain_tests.cpp     17 domain checks
│   ├── analytics_tests.cpp  30 analytics checks
│   ├── unit_console.cpp     36 native cart / CRUD checks (Windows)
│   ├── native_smoke.py      9 piped-input integration scenarios
│   └── site_check.py        99 headless browser checks (file:// and http://)
├── tools/                   prepare_demo.py, record_evidence.py, render_figures.py, build_site_data.py
├── data/
│   ├── sample/              Versioned demo seed
│   ├── legacy/              Original text data
│   └── runtime/             Mutable local copy (git-ignored)
├── reports/                 Recorded CLI output, native transcripts, test reports, catalog.json
├── assets/
│   ├── screenshots/         30 screenshots + manifest.json (shared by README and webpage)
│   ├── js/data.js           Bundled catalog + screenshot list (generated)
│   ├── css/ · js/           Webpage style and behaviour
│   └── favicon.svg
├── docs/                    Architecture, demo guide, requirements coverage, original brief, history
├── .vscode/tasks.json       Build, test, run, preview and refresh tasks
├── CMakeLists.txt           C++17 build with 7 CTest targets
├── Shopping_Portal.sln      Visual Studio solution (+ .vcxproj)
├── build.bat · run.bat      Windows build and launch scripts
└── index.html               Project webpage
```

## 🚀 Setup

Run every command from the **repository root** (the folder with `CMakeLists.txt`) in **PowerShell**, **cmd** or the **VS Code terminal** (`` Ctrl+` ``). The `.\` paths are written for PowerShell and also work in cmd.

### 1. Quick start (Windows)

Requirements: CMake 3.20+, a C++17 compiler (Visual Studio 2022 *Desktop development with C++* or MinGW-w64 g++) and Python 3.10+.

```powershell
.\build.bat      # configure, build and run CTest
.\run.bat        # reset demo data and start the portal
```

The same steps by hand:

```powershell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
python tools/prepare_demo.py
.\run.bat
```

With MinGW + Ninja:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

In Visual Studio, open `Shopping_Portal.sln`, run `python tools/prepare_demo.py` once and set the debugger working directory to `$(ProjectDir)data\runtime`.

### 2. Linux / macOS (CLI and C++ tests)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/catalog_cli --data data/sample --stats
```

The role-based console UI uses the Windows console API, so `Shopping_Portal.exe` is built only on Windows.

### 3. Webpage

Double-click `index.html`; no server is needed. To preview it exactly as GitHub Pages serves it:

```powershell
python -m http.server 8080 --bind 127.0.0.1
```

Then open <http://127.0.0.1:8080>, and press `Ctrl+C` to stop the server.

### 4. Reset, re-record and re-render

```powershell
python tools/prepare_demo.py --reset          # restore runtime data
python tests/native_smoke.py                  # integration scenarios
python tools/record_evidence.py               # refresh reports/ and assets/js/data.js
python -m pip install pillow playwright
python tools/render_figures.py --browser      # refresh assets/screenshots/, data.js and the gallery
python tests/site_check.py                    # webpage checks (file:// and http://)
```

## 📋 Commands reference

| Command | Where to run | What it does and why | Time | Needed? |
| --- | --- | --- | --- | --- |
| `.\build.bat` | Repo root · cmd / PowerShell / VS Code | Configures CMake (MinGW Makefiles when `mingw32-make` exists), builds Release, runs CTest | 1–4 min first time, seconds after | **Required** once |
| `cmake -S . -B build` | Repo root · any terminal | Configure: writes build files into `build/` | 5–30 s | **Required** (or build.bat) |
| `cmake --build build --config Release` | Repo root · any terminal | Compiles the portal, `catalog_cli` and the 3 test programs | 1–3 min first time, seconds after | **Required** (or build.bat) |
| `ctest --test-dir build -C Release --output-on-failure` | Repo root · any terminal | Runs the 7 CTest checks | 1–10 s | Recommended |
| `python tools/prepare_demo.py --reset` | Repo root · any terminal | Copies `data/sample` to `data/runtime` for a fresh demo (run.bat already does this) | < 1 s | Optional |
| `.\run.bat` | Repo root · cmd / PowerShell / VS Code | Builds if needed, prepares data, starts the interactive portal | Interactive | **Required** to use the app |
| `.\build\catalog_cli.exe --summary` (+ other options) | Repo root · any terminal | Read-only reports, statistics, restock plans, quotes, JSON/CSV | < 1 s | Optional |
| `python tests/native_smoke.py` | Repo root · any terminal | 9 end-to-end scenarios against the real executable in a temp copy of the data | 2–15 s | Optional |
| `python tools/record_evidence.py` | Repo root · any terminal | Re-runs CLI, tests and native scenarios; saves `reports/`; rebuilds `assets/js/data.js` | ~10 s | Optional (after code changes) |
| `python tools/render_figures.py --browser` | Repo root · any terminal | Renders the 30 screenshots (Pillow) and the 2 browser captures (Playwright); refreshes data.js and the gallery | 15–40 s | Optional (after changes) |
| `python tools/build_site_data.py` | Repo root · any terminal | Bundles catalog + screenshot list into `assets/js/data.js` and rewrites the static gallery in `index.html` | < 1 s | Optional (called by the two above) |
| `python tests/site_check.py` | Repo root · any terminal | 99 headless-browser checks over `file://` and `http://`, desktop and mobile, both themes | 30–60 s | Optional |
| `python -m http.server 8080 --bind 127.0.0.1` | Repo root · any terminal | Serves the page like GitHub Pages; stop with `Ctrl+C` | Until stopped | Optional |

MinGW builds place programs in `build\`, Visual Studio builds in `build\Release\`, and Linux/macOS in `./build/`. Playwright can use an installed Chrome or Edge; `python -m playwright install chromium` is only needed if neither is installed. The same tasks are available in VS Code under **Terminal → Run Task**.

## 🎬 Demo walkthrough

| Step | Input | Expected result |
| --- | --- | --- |
| 1 | `run.bat`, then **3 → 2**, sign in `rohaan` / `123456` | Customer view |
| 2 | **1 → 1** (Groceries), item **1** (Apples), quantity **2**, then **0**, **0** | One cart line: 2 × Apples |
| 3 | **6** (Show Bill / Checkout), then **1** | PKR 300.00 + 15.00 tax + 15.00 delivery = **PKR 330.00**; order saved |
| 4 | **8** order history, **7** wishlist | New order listed; wishlist saved |
| 5 | **9** logout, **4** back, then **1 → 2**, `admin1` / `admin12` | Admin dashboard |
| 6 | **5** Inventory Insights | Category medians and values, 71 low-stock products, restock estimate |
| 7 | **4** Orders & Stats | Orders with exact total revenue and average order value |
| 8 | Vendor: `vendor1` / `vendor1`, **1 → 1** | Products owned by FastGoods Co |
| 9 | Webpage → Live Demo, add 2 Apples, code **ROHAAN10** | Total PKR 330.00, then PKR 297.00 with 10% off |

Apples start at 20 units in the sample and drop to 18 after step 3. `python tools/prepare_demo.py --reset` restores the runtime for another run. The demo accounts are public sample data. More detail: [docs/DEMO_GUIDE.txt](docs/DEMO_GUIDE.txt).

## 🔍 Feature walkthroughs

**Customer checkout**
1. Welcome screen **3 → 2**, sign in as `rohaan` / `123456`; `Customer::login()` checks `customer_account_save.txt`.
2. **1 → 1** opens Groceries; `Item::print_Items_Menu()` lists products with live stock.
3. Choose Apples, quantity 2. `Cart::add_item()` merges it with any Apples line at the same price. Stock is *not* changed yet.
4. **6** shows the bill: `Cart::Bill()` converts prices to paisa and calls `shop::calculate()`: 300.00 + 15.00 + 15.00 = **PKR 330.00**.
5. **1** confirms. `validateAndDeductStock()` re-reads stock, rejects shortages or ambiguous names, and commits `Groceries.txt` and `orders.txt` together. Apples go 20 → 18.
6. The order gets the next ID after the highest saved one and appears under **8** (Order History).

**Admin inventory insights**
1. Welcome screen **1 → 2**, sign in as `admin1` / `admin12`, choose **5) Inventory Insights**.
2. `shop::loadCatalog(".")` loads the catalog; `priceStatistics()` prints items, median price and stock value per category.
3. `select()` with `maxStock = 5` finds 71 low-stock products, sorted lowest first (first 8 listed).
4. `restockPlan(catalog, 10)` estimates 130 products, 584 units and PKR 6,366,390.00. Nothing is written.

**Vendor stock management**
1. Welcome screen **2 → 2**, sign in as `vendor1` / `vendor1` (FastGoods Co).
2. **1 → 1** lists only products mapped to this vendor in `vendor_products.txt`; add, update and remove check ownership first.

**CLI price quote**
1. `catalog_cli --quote "Apples:3,Milk:2,Bread:1" --discount 10`
2. `parsePercent("10")` → 1000 basis points (max 50%); `buildQuote()` matches names, merges repeats and checks stock.
3. Gross 930.00 − 93.00 = 837.00 + 41.85 tax + 41.85 delivery = **PKR 920.70**. Unknown, ambiguous, zero-quantity or oversold items exit with code 2.

## 💻 catalog_cli reference

```text
catalog_cli [--data DIR] [filters] [view] [output]

Filters   --search TEXT  --category NAME  --min-price PKR  --max-price PKR
          --low-stock  --threshold N
Ordering  --sort name|price|stock|value  --desc  --limit N
Views     --summary  --stats  --categories  --restock TARGET
          --quote "Name:qty,Name:qty"  --discount PCT
Output    --json  --csv  --help
```

| Example | Result |
| --- | --- |
| `catalog_cli --search oil` | 7 products across 4 categories |
| `catalog_cli --category Electronics --sort price` | 21 electronics, cheapest first |
| `catalog_cli --low-stock --sort stock` | Products with 5 or fewer units |
| `catalog_cli --stats` | Min, median, mean and max price per category |
| `catalog_cli --restock 8 --category Electronics` | 11 products, 40 units, PKR 1,509,000.00 |
| `catalog_cli --quote "Apples:3,Milk:2,Bread:1" --discount 10` | 930.00 − 93.00 + 41.85 + 41.85 = **PKR 920.70** |
| `catalog_cli --sort value --desc --limit 12 --csv` | Top 12 stock-value lines as CSV |
| `catalog_cli --quote Apples:999` | `Catalog error: Insufficient stock for Apples: requested 999, available 20` (exit 2) |

The tool is read-only: it never opens account, cart, order or stock files for writing.

## 🧮 Billing and data rules

1. Prices are parsed into integer **paisa** (100 paisa = PKR 1); no floating point is used for money.
2. `subtotal = Σ unit_price × quantity`, minus any quote discount (rounded half up).
3. `tax = delivery = floor((subtotal × 500 + 5000) / 10000)`: 5% each, rounded half up, computed with a quotient/remainder split to avoid overflow.
4. `total = subtotal + tax + delivery`, shown and stored with two decimals.
5. Checkout validates every cart line, stages the changed category files and the new order together, keeps backups and restores them if a write fails. The cart is cleared only after success.
6. Product records use `Name - price PKR, quantity`; parsing uses the final separator and final comma, so names like `T-shirts` stay whole. Malformed rows, unsafe category names, duplicates, invalid prices and negative stock are rejected.
7. Orders are stored as `id|user|total|timestamp|Name:qty;...`; new IDs follow the highest existing ID.

## ✅ Testing and results

| Suite | Checks | Report |
| --- | --- | --- |
| `domain_tests` – parsing, money, rounding, safety, rollback | 17 | [domain_tests.txt](reports/domain_tests.txt) |
| `analytics_tests` – statistics, filters, restock, quotes, CSV | 30 | [analytics_tests.txt](reports/analytics_tests.txt) |
| `console_tests` – native cart and CRUD rules | 36 | [console_tests.txt](reports/console_tests.txt) |
| `native_smoke.py` – end-to-end piped scenarios | 9 | [smoke_tests.txt](reports/smoke_tests.txt) |
| `site_check.py` – webpage over `file://` and `http://`, desktop and mobile, light and dark | 99 | [site_check.py](tests/site_check.py) |
| **Total** | **191** | CTest runs 7 targets: [ctest.txt](reports/ctest.txt) |

| Scenario | Observed result | Evidence |
| --- | --- | --- |
| Catalog validation | 351 products · 12 categories · 4,651 units | [summary.txt](reports/summary.txt) |
| Native checkout, 2 Apples | PKR 330.00; Apples 20 → 18 | [native_checkout.txt](reports/native_checkout.txt) |
| Admin inventory insights | 71 low-stock products; restock to 10 units ≈ PKR 6,366,390.00 | [native_insights.txt](reports/native_insights.txt) |
| Discounted quote | PKR 920.70 | [quote.txt](reports/quote.txt) |
| Restock plan (Electronics → 8) | 11 products · 40 units · PKR 1,509,000.00 | [restock_plan.txt](reports/restock_plan.txt) |
| Overselling a quote | Rejected, exit code 2 | [ctest.txt](reports/ctest.txt) |

### Test coverage matrix

| Area | Domain | Analytics | Console | Smoke | CTest CLI | Webpage |
| --- | :---: | :---: | :---: | :---: | :---: | :---: |
| Product parsing and catalog validation | ✔ | | ✔ | | ✔ | |
| Money parsing and 5% half-up rounding | ✔ | ✔ | ✔ | | ✔ | ✔ |
| Rollback commit and staging failures | ✔ | | ✔ | | | |
| Duplicate registration rejected | | | | ✔ | | |
| Cart merge, edit, remove | | | ✔ | ✔ | | ✔ |
| Checkout, stock deduction, order record | | | ✔ | ✔ | | ✔ |
| Wishlist persistence, search de-duplication | | | | ✔ | | |
| Category and product CRUD guards | | | ✔ | ✔ | | |
| Statistics, filters, deterministic sorting | | ✔ | | | ✔ | ✔ |
| Restock plans, quotes, oversell rejection, CSV | | ✔ | | ✔ | ✔ | |
| Navbar, themes, mobile menu, gallery, `file://` | | | | | | ✔ |

## 🌐 Webpage

[`index.html`](index.html) ([live](https://rohaan2802.github.io/Shopping_Portal/)) presents the project in twelve navbar sections: **Overview, Features, Architecture, Classes, Walkthroughs, Algorithms, Insights, Live Demo, CLI, Screenshots, Setup, Testing**, plus an author footer. It works when opened directly from disk (`file://`) and on GitHub Pages: all data comes from `assets/js/data.js` and the screenshots are plain `<img>` tags.

- **Navbar:** right-aligned, highlighted active section, scroll-progress bar, GitHub link, sun/moon theme icon; below 1560 px wide it becomes a visible menu (hamburger) button that turns into ✕ when open.
- **Themes:** *Light* uses warm stone `#f5f3ee` with indigo `#4f46e5` → violet `#7c3aed` and teal `#0f766e`. *Dark* uses deep navy `#0b1020` with periwinkle `#8b93ff`, lavender `#c084fc` and aqua `#2dd4bf`. The choice is saved and follows the system setting on first visit.
- **Typography:** 18 px body text (16.5 px on phones) with headings, navbar links, cards, tables and code scaled to match.
- **Insights:** KPIs, inventory value by category, a price-statistics table (min, median, mean, max) and the lowest-stock list, computed live from the bundled catalog.
- **Live Demo:** search, filter, sort, in-stock toggle, quantity steppers, discount codes (`ROHAAN10`, `FAST5`) and a simulated receipt, using the same billing rules as the C++ code.
- **Screenshots:** the same 30 images as this README, with search, category filters and a full-screen viewer (keyboard, swipe, zoom).
- **Back to top:** a rounded button with a double-chevron icon and a ring that fills as you scroll.

## 🖼️ Screenshots

All 30 screenshots are 1600 × 1000 and show real program output, recorded transcripts, source code or browser captures. Each one sits on a coloured background for its category (Catalog, Analytics, Native, Tests, Source, Browser) so it stays clear in light and dark themes. The webpage gallery uses the same files in the same order.

### 1. Catalog audit summary

<p align="left"><img src="assets/screenshots/01_catalog_audit_summary.png" alt="Catalog audit summary" width="900"></p>

catalog_cli --summary validates the seed and totals products, units and value for every category. *Source:* `reports/summary.txt`

### 2. Inventory value by category

<p align="left"><img src="assets/screenshots/02_inventory_value_by_category.png" alt="Inventory value by category" width="900"></p>

Stock value (price x units) per category from the exported catalog; product counts in brackets. *Source:* `reports/catalog.json`

### 3. Product search

<p align="left"><img src="assets/screenshots/03_product_search.png" alt="Product search" width="900"></p>

A case-insensitive search for 'oil' finds matches across several categories with full names, prices and stock. *Source:* `reports/search.txt`

### 4. Electronics sorted by price

<p align="left"><img src="assets/screenshots/04_electronics_sorted_by_price.png" alt="Electronics sorted by price" width="900"></p>

Category filter plus ascending price sort, with a deterministic name tie-breaker. *Source:* `reports/price_sort.txt`

### 5. Low-stock review

<p align="left"><img src="assets/screenshots/05_low_stock_review.png" alt="Low-stock review" width="900"></p>

Products at or below the low-stock threshold (default 5), lowest stock first. *Source:* `reports/low_stock.txt`

### 6. Price statistics per category

<p align="left"><img src="assets/screenshots/06_price_statistics_per_category.png" alt="Price statistics per category" width="900"></p>

New --stats view: min, median, mean and max unit price for each category and overall. *Source:* `reports/price_stats.txt`

### 7. Restock plan with cost

<p align="left"><img src="assets/screenshots/07_restock_plan_with_cost.png" alt="Restock plan with cost" width="900"></p>

New --restock view: units to order and estimated cost to bring Electronics up to 8 units each. *Source:* `reports/restock_plan.txt`

### 8. Discounted price quote

<p align="left"><img src="assets/screenshots/08_discounted_price_quote.png" alt="Discounted price quote" width="900"></p>

New --quote view: a stock-checked basket with 10% off applied before 5% tax and 5% delivery. *Source:* `reports/quote.txt`

### 9. CSV export of top stock value

<p align="left"><img src="assets/screenshots/09_csv_export_of_top_stock_value.png" alt="CSV export of top stock value" width="900"></p>

--sort value --desc --limit 12 --csv exports the most valuable stock lines for spreadsheets. *Source:* `reports/top_value.csv`

### 10. Native role selection

<p align="left"><img src="assets/screenshots/10_native_role_selection.png" alt="Native role selection" width="900"></p>

The Windows console portal opens with Admin, Vendor and Customer entry points. *Source:* `reports/native_welcome.txt`

### 11. Admin dashboard

<p align="left"><img src="assets/screenshots/11_admin_dashboard.png" alt="Admin dashboard" width="900"></p>

Admin login reaches the dashboard, now with the Inventory Insights option. *Source:* `reports/native_admin.txt`

### 12. Admin inventory insights

<p align="left"><img src="assets/screenshots/12_admin_inventory_insights.png" alt="Admin inventory insights" width="900"></p>

New native report: category medians and values, low-stock alerts and a restock estimate. *Source:* `reports/native_insights.txt`

### 13. Vendor product ownership

<p align="left"><img src="assets/screenshots/13_vendor_product_ownership.png" alt="Vendor product ownership" width="900"></p>

A vendor sees only the products it owns, with price and stock. *Source:* `reports/native_vendor.txt`

### 14. Browsing the grocery catalog

<p align="left"><img src="assets/screenshots/14_browsing_the_grocery_catalog.png" alt="Browsing the grocery catalog" width="900"></p>

Customers browse a category with live prices and stock read from the data files. *Source:* `reports/native_browse.txt`

### 15. Merged cart and line totals

<p align="left"><img src="assets/screenshots/15_merged_cart_and_line_totals.png" alt="Merged cart and line totals" width="900"></p>

Adding Apples twice merges into one line (qty 3); Milk is a second line with its own total. *Source:* `reports/native_cart.txt`

### 16. Confirmed checkout bill

<p align="left"><img src="assets/screenshots/16_confirmed_checkout_bill.png" alt="Confirmed checkout bill" width="900"></p>

2 Apples: PKR 300.00 subtotal + 15.00 tax + 15.00 delivery = PKR 330.00, then the order is saved. *Source:* `reports/native_checkout.txt`

### 17. Saved order and updated stock

<p align="left"><img src="assets/screenshots/17_saved_order_and_updated_stock.png" alt="Saved order and updated stock" width="900"></p>

The order log gains the new order and Apples stock drops from 20 to 18. *Source:* `reports/checkout_orders.txt`

### 18. Persistent wishlist

<p align="left"><img src="assets/screenshots/18_persistent_wishlist.png" alt="Persistent wishlist" width="900"></p>

Wishlist items are saved per customer and survive logout. *Source:* `reports/native_wishlist.txt`

### 19. Domain correctness checks

<p align="left"><img src="assets/screenshots/19_domain_correctness_checks.png" alt="Domain correctness checks" width="900"></p>

17 checks: money parsing, rounding, malformed rows, unsafe names and rollback cleanup. *Source:* `reports/domain_tests.txt`

### 20. Analytics test suite

<p align="left"><img src="assets/screenshots/20_analytics_test_suite.png" alt="Analytics test suite" width="900"></p>

30 new checks for statistics, filters, sorting, restock plans, quotes, discounts and CSV. *Source:* `reports/analytics_tests.txt`

### 21. Native cart and CRUD checks

<p align="left"><img src="assets/screenshots/21_native_cart_and_crud_checks.png" alt="Native cart and CRUD checks" width="900"></p>

Unit checks for cart merging, stock rules and category-gated product operations. *Source:* `reports/console_tests.txt`

### 22. Integration scenarios

<p align="left"><img src="assets/screenshots/22_integration_scenarios.png" alt="Integration scenarios" width="900"></p>

Nine piped-input scenarios run the real executable in temporary fixtures. *Source:* `reports/smoke_tests.txt`

### 23. CMake and CTest run

<p align="left"><img src="assets/screenshots/23_cmake_and_ctest_run.png" alt="CMake and CTest run" width="900"></p>

All CTest targets pass, including CLI quote, statistics and overselling checks. *Source:* `reports/ctest.txt`

### 24. Complete-record parsing

<p align="left"><img src="assets/screenshots/24_complete_record_parsing.png" alt="Complete-record parsing" width="900"></p>

Parsing uses the final separators, so names like T-shirts survive, and validates every field. *Source:* `include/Commerce.h`

### 25. Exact PKR charge calculation

<p align="left"><img src="assets/screenshots/25_exact_pkr_charge_calculation.png" alt="Exact PKR charge calculation" width="900"></p>

Integer-paisa arithmetic: tax and delivery are rounded half up without overflow. *Source:* `include/Commerce.h`

### 26. Rollback-capable file commit

<p align="left"><img src="assets/screenshots/26_rollback_capable_file_commit.png" alt="Rollback-capable file commit" width="900"></p>

Checkout stages every file, keeps backups and restores them if any step fails. *Source:* `include/Commerce.h`

### 27. Quote builder with stock checks

<p align="left"><img src="assets/screenshots/27_quote_builder_with_stock_checks.png" alt="Quote builder with stock checks" width="900"></p>

Analytics.h merges repeated items, rejects unknown or ambiguous names and checks stock. *Source:* `include/Analytics.h`

### 28. Inventory insights source

<p align="left"><img src="assets/screenshots/28_inventory_insights_source.png" alt="Inventory insights source" width="900"></p>

The admin report reuses the analytics layer and never writes to data files. *Source:* `src/Admin.cpp`

### 29. Interactive shopping sandbox

<p align="left"><img src="assets/screenshots/29_interactive_shopping_sandbox.png" alt="Interactive shopping sandbox" width="900"></p>

The webpage loads the C++ catalog export: search, filter, sort and add items to the cart. *Source:* `index.html`

### 30. Simulated receipt with discount

<p align="left"><img src="assets/screenshots/30_simulated_receipt_with_discount.png" alt="Simulated receipt with discount" width="900"></p>

The ROHAAN10 code takes 10% off before tax and delivery, matching the CLI quote rules. *Source:* `index.html`

## ⚠️ Limitations

- Demo accounts are stored in plain text and there is no password hashing, payment gateway or shipping integration; the browser sandbox is a simulation.
- Persistence uses single-process text files. Checkout rolls back ordinary file errors, but there is no database locking or concurrent-writer protection.
- Native cart identity is product name + price; names that are ambiguous across categories are rejected rather than guessed.
- Fixed console capacities (500 cart/product slots, 99 displayed categories) match the course project; long labels may be truncated on screen but stay complete in files and CLI output.
- The original course brief is broader (manager roles, purchasing ledgers, delivery tracking). See [docs/REQUIREMENTS_REVIEW.md](docs/REQUIREMENTS_REVIEW.md).

## 👨‍💻 About the author

**Mohammad Rohaan** – BS Computer Science, FAST-NUCES. Built as an object-oriented programming project (22I-2327, Section Z) and extended with analytics, testing, tooling and a project webpage.

[![GitHub](https://img.shields.io/badge/GitHub-rohaan2802-181717?style=for-the-badge&logo=github&logoColor=white)](https://github.com/rohaan2802)
[![Email](https://img.shields.io/badge/Email-m.rohaanarshad%40gmail.com-D14836?style=for-the-badge&logo=gmail&logoColor=white)](mailto:m.rohaanarshad@gmail.com)
[![Webpage](https://img.shields.io/badge/Webpage-Shopping__Portal-4F46E5?style=for-the-badge&logo=googlechrome&logoColor=white)](https://rohaan2802.github.io/Shopping_Portal/)
