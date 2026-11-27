# Original brief and maintained scope

The preserved [course brief](OOP_Project_Description.pdf) describes a broader retail system than the original repository implements. This project implements the three-role shopping application at its core and extends it with analytics and tooling. The table maps each area of the ten-page brief to the implementation.

| Brief area | Maintained implementation | Coverage |
| --- | --- | --- |
| OOP abstractions and role inheritance | Abstract `Role` with Admin, Vendor and Customer subclasses; encapsulated Cart and Item operations | Present; concrete roles are used directly by the main menu |
| Customer cart and invoice totals | Add/merge, remove, modify quantity, display, confirmed checkout and timestamped order records | Present; decimal billing and stock/order save safety improved |
| Inventory and categories | Registered categories, product CRUD, stock, search and Vendor ownership | Present for the teaching data model |
| Saved customer orders | Customer history and Admin order views | Present; isolated checkout evidence included |
| Search and stock review | Native search plus CLI filters, price ranges, sorting, low-stock output, JSON and CSV export | Present and verified on sample inputs |
| Inventory reporting | Admin Inventory Insights, `--stats`, `--summary` and `--restock` with estimated purchase cost | Present; price-based, without purchase-cost accounting |
| Full customer profile and guest-cart preservation | Username/password account pairs; login precedes native shopping | Partial; full address/email/card profile and guest checkout are not implemented |
| Separate Store/Inventory Manager roles | Admin carries inventory responsibilities | Not separate role implementations |
| SKU, department and purchase-price model | Native product identity is name + price within category data | Not implemented as the brief's full SKU/accounting schema |
| Purchase invoices, credit ledger and cash register | No purchasing/payment ledger | Not implemented |
| Vendor supply requests and acceptance/rejection | Vendor product ownership and stock management | Supply-request lifecycle not implemented |
| Premium-user discount and top-ten sales | Percentage discounts on quotes (CLI) and sandbox codes; top stock-value ranking via `--sort value --desc` | Partial; no frequency-based premium engine or sales ranking |
| Dispatch, delivery status, card/COD lifecycle | Saved order confirmation only | Not implemented; no payment service is simulated as real |
| Daily/weekly/monthly/yearly profit reports | Catalog audit and order history | Full cost/profit and purchasing reports are not implemented |

## Additions beyond the original repository

- A standard-library C++17 catalog CLI and shared domain helpers.
- Exact integer-paisa charge calculation and two-decimal display/storage.
- Complete-line parsing for hyphenated product names and safe category filenames.
- Checkout staging/backup recovery for ordinary file-operation failures.
- Current-password verification, maximum-ID order allocation and clean piped input/EOF behavior.
- Read-only maintained seed, isolated runtime and tests, actual saved reports.
- `Analytics.h`: price statistics, filters, value sorting, restock plans, stock-checked quotes with discounts and CSV export (30 tests).
- Admin Inventory Insights and exact-paisa revenue with average order value.
- Browser shopping sandbox with discount codes, live inventory insights and a 30-screenshot gallery shared with the README; the page works from `file://` and GitHub Pages because its data is bundled in `assets/js/data.js`.

The README's limitations describe the persistence and authentication boundaries. The browser sandbox is a presentation feature and does not satisfy missing native accounting or payment requirements.
