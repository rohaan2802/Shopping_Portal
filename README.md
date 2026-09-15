# FAST Shopping Portal

Windows **console OOP shopping application** with working **Admin**, **Vendor**, and **Customer** roles. Colorful spacious UI (`Other_Fun` style), file-backed catalogs, cart with tax/delivery billing, wishlist, and order history.

**Author:** Mohammad Rohaan · **22I-2327** · **Sec-Z** · [rohaan2802](https://github.com/rohaan2802)

FAST Shopping Portal is a course OOP project upgraded to a professional real-world console workflow:

- Role-based entry: Admin / Vendor / Customer / Exit
- Persistent text-file data (accounts, stock, orders)
- Full customer shopping loop: browse → search → cart → bill → checkout → history
- Admin catalog management + user overview + revenue stats
- Vendor registration and ownership-scoped stock management

Assignment notes live in [`docs/Classes.txt`](docs/Classes.txt) and [`docs/OOP_Project_Description.pdf`](docs/OOP_Project_Description.pdf).

---

## How to run

**Requirements:** Windows + Visual Studio Build Tools / MSVC (`cl.exe`)

```bat
build.bat
run.bat
```

Or open `Shopping_Portal.sln` in Visual Studio and run with the project folder as the working directory (so `*.txt` catalogs resolve).

---

## Features by role

**Demo accounts**

| Role | Username | Password | Notes |
|------|----------|----------|-------|
| Admin | *(none seeded)* | — | First-time Admin must **Register once** (only one admin allowed), then use **Login** |
| Vendor | `vendor1` | `vendor1` | Or register a new vendor |
| Customer | `rohaan` | `123456` | Or register a new customer |

### Admin
- Portal menu: **Registration / Login / Back** (same pattern as Customer/Vendor)
- Only **one** admin account can be created for the whole portal (starts with no admin)
- First run: choose Registration once, then Login only
- View customers, vendors, and the admin
- Manage categories (add / remove)
- Add, update, and remove products per category
- View orders and revenue statistics

### Vendor
- Register (username, password, company) or login
- Add / update / remove **own** products (tracked in `vendor_products.txt`)
- Browse categories and search the catalog
- Logout

### Customer
- Register / login / forgot-password (validated 6–16 chars, no spaces)
- Browse 12 categories and add to cart (**stock is not deducted yet**)
- Search catalog + cart
- Modify cart quantities (validates against available stock; no file write)
- Remove items / logout / clear cart (**stock files unchanged**)
- Bill with **5% tax + 5% delivery**
- Checkout validates stock, **deducts once**, then writes `orders.txt`
- Wishlist add/remove/view — **persisted** in `wishlist.txt` (survives exit/reopen)
- Order history from `orders.txt`
- Logout clears cart only

---

## Screenshot gallery

Console UI mockups covering every major feature (SS01–SS35). Regenerate anytime with: `python scripts/generate_screenshots.py` (Pillow).

### SS01 — Welcome Banner
![SS01](docs/screenshots/ss01.png)

### SS02 — Role Selection Menu
![SS02](docs/screenshots/ss02.png)

### SS03 — Admin Login Prompt
![SS03](docs/screenshots/ss03.png)

### SS04 — Admin Login Success
![SS04](docs/screenshots/ss04.png)

### SS05 — Admin Dashboard
![SS05](docs/screenshots/ss05.png)

### SS06 — Admin — View All Users
![SS06](docs/screenshots/ss06.png)

### SS07 — Admin — Manage Categories
![SS07](docs/screenshots/ss07.png)

### SS08 — Admin — Add Category Success
![SS08](docs/screenshots/ss08.png)

### SS09 — Admin — Manage Products
![SS09](docs/screenshots/ss09.png)

### SS10 — Admin — View Category Products
![SS10](docs/screenshots/ss10.png)

### SS11 — Admin — Orders & Statistics
![SS11](docs/screenshots/ss11.png)

### SS12 — Vendor Portal Gateway
![SS12](docs/screenshots/ss12.png)

### SS13 — Vendor Registration
![SS13](docs/screenshots/ss13.png)

### SS14 — Vendor Login Success
![SS14](docs/screenshots/ss14.png)

### SS15 — Vendor Dashboard
![SS15](docs/screenshots/ss15.png)

### SS16 — Vendor — Stock Management
![SS16](docs/screenshots/ss16.png)

### SS17 — Vendor — My Products List
![SS17](docs/screenshots/ss17.png)

### SS18 — Customer Gateway
![SS18](docs/screenshots/ss18.png)

### SS19 — Customer Registration
![SS19](docs/screenshots/ss19.png)

### SS20 — Customer Login (Masked Password)
![SS20](docs/screenshots/ss20.png)

### SS21 — Forgot Password Flow
![SS21](docs/screenshots/ss21.png)

### SS22 — Customer Main Menu
![SS22](docs/screenshots/ss22.png)

### SS23 — Browse Categories
![SS23](docs/screenshots/ss23.png)

### SS24 — Category Product Listing
![SS24](docs/screenshots/ss24.png)

### SS25 — Add to Cart Success
![SS25](docs/screenshots/ss25.png)

### SS26 — Display Cart Items
![SS26](docs/screenshots/ss26.png)

### SS27 — Modify Cart Quantity
![SS27](docs/screenshots/ss27.png)

### SS28 — Search Products Across Catalog
![SS28](docs/screenshots/ss28.png)

### SS29 — Remove Item From Cart
![SS29](docs/screenshots/ss29.png)

### SS30 — Bill With Tax and Delivery
![SS30](docs/screenshots/ss30.png)

### SS31 — Checkout — Order Placed
![SS31](docs/screenshots/ss31.png)

### SS32 — Wishlist Feature
![SS32](docs/screenshots/ss32.png)

### SS33 — Order History
![SS33](docs/screenshots/ss33.png)

### SS34 — Input Validation Error
![SS34](docs/screenshots/ss34.png)

### SS35 — Logout and Exit
![SS35](docs/screenshots/ss35.png)

---

## Architecture

```text
Main.cpp                 Role picker → Admin | Vendor | Customer
Other_Fun.h              Console colors, banners, validated input helpers
Role.h                   Abstract registraion() + login()
Admin.h/.cpp             Admin auth (Reg/Login/Back) + dashboard ops
Vendor.h/.cpp            Vendor auth + owned stock management
Customer.h/.cpp          Auth, menus, wishlist, checkout, history
Items.h/.cpp             Category/product I/O + browse/search UI
Cart.h/.cpp              Cart arrays, Search, Modify, Bill, stock deduct at checkout
```

OOP inheritance: `Admin` / `Vendor` / `Customer` implement `Role`. Catalog lines keep the original format:

```text
Apples - 150 PKR, 2
```

---

## Menus / keymap

| Screen | Keys |
|--------|------|
| Role select | `1` Admin · `2` Vendor · `3` Customer · `4` Exit |
| Admin portal | `1` Register · `2` Login · `3` Back |
| Customer gateway | `1` Register · `2` Login · `3` Forgot password · `4` Back |
| Customer view | `1` Order · `2` Modify · `3` Cart · `4` Search · `5` Remove · `6` Checkout · `7` Wishlist (saved) · `8` History · `9` Logout |
| Admin dashboard | `1` Users · `2` Categories · `3` Products · `4` Orders/Stats · `5` Logout |
| Vendor dashboard | `1` Stock · `2` Categories · `3` Search · `4` Logout |

---

## Data files

| File | Purpose |
|------|---------|
| `ItemsCategory.txt` | Category list |
| `<Category>.txt` | Stock lines (`Name - price, qty`) |
| `customer_account_save.txt` | Customer username/password pairs |
| `admin_accounts.txt` | Sole admin credentials (empty until first Registration) |
| `vendor_accounts.txt` | Vendor username / password / company |
| `vendor_products.txt` | Vendor ownership (`user\|cat\|name\|price\|qty`) |
| `orders.txt` | Placed orders (`id\|user\|total\|details`) |
| `wishlist.txt` | Persisted wishlist (`username\|item_name`; comment header ok) |
| `temp.txt` | Scratch file during rewrites (gitignored) |

---

## Build notes

- `build.bat` locates `vcvars64.bat`, compiles with `/std:c++17` and `/D_HAS_STD_BYTE=0` (avoids Windows `byte` clash).
- Output: `build\Shopping_Portal.exe`
- Screenshots regenerable via: `python scripts/generate_screenshots.py` (Pillow)

---

## Bugs fixed (vs original stubs)

- Wired **Admin** and **Vendor** (were empty stubs)
- Implemented **`Cart::Search`** and cart **Modify**
- Stock deducts **only at checkout** (add/remove/logout never rewrite category qty)
- Wishlist persists to `wishlist.txt` per user (no duplicates)
- Cleaned Electronics / Home Appliances / Health catalogs
- `cart_size` now starts at **0** (no unused sentinel index)
- Customer login always returns `bool`; search/modify menu options work
- Spacious colorful UI helpers centralized in `Other_Fun.h`
- Validated credentials and integer menu ranges

---

## Author

**Mohammad Rohaan** — 22I-2327 — Sec-Z  
Repository: [https://github.com/rohaan2802/Shopping_Portal](https://github.com/rohaan2802/Shopping_Portal)
