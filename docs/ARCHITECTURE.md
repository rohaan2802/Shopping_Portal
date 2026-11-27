# Engineering walkthrough

## Purpose

Shopping_Portal is a C++17 commerce application organised as a reviewable engineering project. The native console application remains the source of truth for role workflows and file-backed persistence. A separate catalog CLI exposes read-only inspection, and the static webpage provides an independently scoped browser demonstration.

## Entry points

| Entry point | Audience | Writes data? | Main responsibility |
| --- | --- | --- | --- |
| `run.bat` | Windows reviewer | Creates ignored runtime copy | Builds when needed, prepares the sample runtime, launches the native portal |
| `Shopping_Portal.exe` | Native user | Yes, only in `data/runtime` | Admin, Vendor and Customer workflows |
| `catalog_cli` | Reviewer / auditor | No | Validates, filters, sorts, analyses, quotes and exports the catalog |
| `index.html` | Browser reviewer | Browser local storage only | Explains the system and simulates a cart from the bundled catalog (`assets/js/data.js`); works from `file://` and GitHub Pages |

The three entry points deliberately do not share mutable state. A browser receipt is labelled simulated, and the catalog CLI cannot change stock, accounts or orders.

## Data lifecycle

```text
data/sample/ ──copy──> data/runtime/ ──used by──> native portal
     │                    │                         │
     │                    └── ignored local changes  └── orders / wishlist / stock updates
     │
     ├── validated by catalog_cli ──> reports/catalog.json ──> assets/js/data.js ──> webpage
     └── copied to isolated test fixtures ──> reports/*.txt
```

`data/sample` is versioned and read-only during normal review. `tools/prepare_demo.py --reset` restores known sample files to `data/runtime`; normal launch retains the local runtime so a reviewer can inspect a native checkout after it happens. Integration tests and evidence recording use disposable fixtures so they never edit the maintained sample.

## Domain rules

`include/Commerce.h` is the maintained boundary for records that need the same interpretation across the portable audit tool and native application.

- **Money:** values are parsed into signed 64-bit paisa. The application never calculates billing with binary floating-point values.
- **Product records:** the parser uses the final `" - "` and final comma so names such as `T-shirts` remain complete.
- **Catalog safety:** category names reject traversal, reserved Windows device names and invalid characters. Category files and duplicates are validated while loading.
- **Billing:** tax and delivery are each calculated at 5% and independently rounded half up. A PKR 300.00 subtotal yields PKR 15.00 tax, PKR 15.00 delivery and PKR 330.00 total.
- **Checkout:** all lines are validated before a write. Changed stock files and a new order are staged, backups are retained while files are installed, and ordinary file-operation failures trigger restoration.

## Analytics layer

`include/Analytics.h` builds on `Commerce.h` and is shared by the CLI, the native admin dashboard and `analytics_tests`. Every function is read-only.

| Function | Purpose |
| --- | --- |
| `priceStatistics` | Per-category and overall min, median, mean and max price, units and inventory value |
| `select` + `Filter` | Name search, category, inclusive price range and low-stock threshold |
| `sortProducts` | Name, price, stock or stock value, ascending or descending, with a name tie-breaker |
| `restockPlan` | Units and estimated cost to raise every product below a target to that target |
| `buildQuote` | Parses `Name:qty,...`, merges repeats, rejects unknown/ambiguous names and overselling, applies a discount before tax |
| `toCsv`, `csvField` | Spreadsheet export with quoted commas and escaped quotes |

## Role responsibility map

| Role | Typical actions | Source |
| --- | --- | --- |
| Admin | Manage categories and products; order statistics with exact revenue and average order value; inventory insights (category medians, low-stock alerts, restock estimate) | `src/Admin.cpp`, `src/Items.cpp` |
| Vendor | Register/login; work with owned product listings | `src/Vendor.cpp` |
| Customer | Browse, cart, checkout, wishlist, history and password changes | `src/Customer.cpp`, `src/Cart.cpp` |
| Catalog reviewer | Search, price/stock filters, statistics, restock plans, quotes, JSON and CSV export | `src/catalog_cli.cpp` |

## Verification layers

1. `domain_tests` checks parsing, safe fields/categories, exact money, charge rounding, multi-file updates and staged-file cleanup.
2. `analytics_tests` (30 checks) covers statistics, filters, sorting, restock plans, quotes, discounts and CSV export, plus the sample seed.
3. `console_tests` checks cart behavior and native category/product rules on a temporary fixture.
4. CTest also runs the CLI against the seed: catalog validation, a PKR 330.00 quote, statistics, and a rejected oversell.
5. `native_smoke.py` drives nine role scenarios through redirected input and checks resulting state, including the read-only admin insights report.
6. `site_check.py` (99 checks) opens the webpage in headless Chrome/Edge both from disk (`file://`) and over HTTP, at desktop and mobile widths in both themes: navbar, icons, back-to-top, 30 screenshots, content in every section, insights tables, gallery filters, sandbox billing and console errors.

Outputs in `reports/` are regenerated after a local build with `python tools/record_evidence.py`, and the screenshots with `python tools/render_figures.py --browser`. Both finish by running `tools/build_site_data.py`.

## Webpage data bundle

Browsers block `fetch()` of local JSON files when a page is opened by double-clicking it (`file://`). The webpage therefore uses no `fetch()`:

- `tools/build_site_data.py` writes `assets/js/data.js` as `window.SHOP_DATA = {catalog, screenshots}`, built from `reports/catalog.json` and `assets/screenshots/manifest.json`.
- The same script writes the 30 gallery cards into `index.html` between `<!-- GALLERY:START -->` and `<!-- GALLERY:END -->` as plain `<figure>`/`<img>` HTML, so screenshots appear even without JavaScript.
- `assets/js/site.js` reads `window.SHOP_DATA` for the insights, price statistics and sandbox, and only adds filtering and the viewer to the static gallery.
- All paths are relative, so the folder works from disk, any static server and GitHub Pages.

## Classes

| Type | Kind | Notes |
| --- | --- | --- |
| `Role` | Abstract base | Pure virtual `registraion()` and `login()`, virtual destructor |
| `Admin : public Role` | Class | One admin account; users, categories, products, orders & stats, inventory insights |
| `Vendor : public Role` | Class | Company name; ownership via `vendor_products.txt` |
| `Customer : protected Role` | Class | 100-slot wishlist, browsing, checkout, history, password change |
| `Cart` | Class | 500-line arrays; merges identical name + price; `validateAndDeductStock()` |
| `Item` | Class (mostly static) | Category registry, product CRUD, de-duplicated search |
| `shop::Product`, `shop::Totals` | Structs | Integer-paisa product and bill values |

## Algorithms

| Algorithm | Complexity | Summary |
| --- | --- | --- |
| Complete-record parsing | O(line) | Last `" - "` and last comma; strict price and quantity validation |
| 5% half-up charge | O(1) | `(s/10000)*bp + ((s%10000)*bp + 5000)/10000` |
| Staged commit with rollback | O(bytes) | `.shop.tmp` → `.shop.bak` swap → delete backups; restore on failure |
| Median / mean by category | O(n log n) | Sort per category; half-up averages |
| Restock plan | O(n log n) | `target − stock` units per product, sorted by stock then name |
| Quote merging | O(k·n) | Case-insensitive match, ambiguity check, merged stock check |
| Deterministic sort | O(n log n) | `std::stable_sort` with lower-case name tie-break |

## Scope boundaries

The project uses public demonstration accounts and text files because it is a local OOP application, not a hosted commerce service. It has no payment processor, delivery integration, password-hashing system, database locking or concurrent-writer guarantee. The requirements coverage review records those boundaries alongside the working functionality.
