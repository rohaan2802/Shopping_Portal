# Shopping_Portal (FAST Shopping Portal)

Windows **console OOP shop**. The working product is the **Customer** path: register / login / cart / billing against category `.txt` catalogs. **Admin** and **Vendor** are menu entries and empty stubs. Banner text: “WELCOME TO FAST SHOPPING PORTAL”.

**Course folder:** `22I-2327 Mohammad Rohaan Sec-Z/` (`Classes.txt`, `OOP Project Description.pdf`)  
**Solution:** `Shopping_Portal.sln`  
**Author:** Mohammad Rohaan (22I-2327) · [rohaan2802](https://github.com/rohaan2802)

---

## Table of contents

1. [What this program does](#what-this-program-does)
2. [Architecture](#architecture)
3. [Entry (`Main.cpp`)](#entry-maincpp)
4. [Role and Customer](#role-and-customer)
5. [Customer menus](#customer-menus)
6. [Item catalogs and file I/O](#item-catalogs-and-file-io)
7. [Cart](#cart)
8. [Windows console styling](#windows-console-styling)
9. [Security](#security)
10. [File-by-file](#file-by-file)
11. [Data files](#data-files)
12. [Build and run](#build-and-run)
13. [Limitations](#limitations)
14. [Author](#author)

---

## What this program does

1. Maximize the console (`ShowWindow(..., SW_MAXIMIZE)`).
2. `Welcome_Message()` then `Select_Role()`: Admin, Vendor, Customer, or close the window.
3. If **Customer (3)**: `Customer::customer_Reg_Log_Menu` — Registration, Login, Go Back.
4. After a successful login: `show_customer_menu` — order from catalogs, display cart, remove lines, show bill.
5. Stock files are rewritten when items are added to the cart (`Item::update_items`).

Admin (1) and Vendor (2) are **not** implemented: `Main.cpp` only constructs `Customer` when the choice is 3. Choice 4 sends `WM_CLOSE`.

---

## Architecture

```text
Main.cpp
  Welcome_Message / Select_Role          Other_Fun.h
  Customer.customer_Reg_Log_Menu()       Customer.cpp
        registraion / login / updateValueInFile
        show_customer_menu
              Item.print_Items_Menu      Items.cpp  →  Cart c
              Item.Display_Cart / Item_Remove_from_Cart / Bill

Role.h          abstract registraion() + login()
Admin.h/.cpp    empty (include only)
Vendor.h/.cpp   empty (include only)
```

Global `Cart c` in `Items.cpp` is shared by `Item` methods. `Cart.h` and `Items.h` include each other; `Item` holds `itemnames`, `Items_Price`, `Items_Quantity` as strings.

`Classes.txt` sketches a different design (`Person`, `Product` SKU, `Inventory`, `Order`, `Admin` with `add_product`). **Shipped code does not use those names.** Treat `Classes.txt` as an early design note, not the runtime model.

---

## Entry (`Main.cpp`)

```text
back:
  maximize console
  Welcome_Message()
  select_menu = Select_Role()
  if 3: Customer cus; if customer_Reg_Log_Menu() == false → goto back
  if 4: PostMessage(WM_CLOSE)
  system("pause")
```

Includes: `Role.h`, `Admin.h`, `Vendor.h`, `Customer.h`, `Items.h`, `Cart.h`, `Other_Fun.h`. Admin/Vendor headers are pulled in but never instantiated.

---

## Role and Customer

**`Role`** (`Role.h`): protected `reg_name`, `reg_password`, `confirm_pass`, `login_name`, `login_pass`. Pure virtual `registraion()` (typo in source) and `login()`.

**`Customer : protected Role`** (`Customer.h` / `Customer.cpp`):

| Method | Behaviour |
|--------|-----------|
| `customer_Reg_Log_Menu` | 1 Registration, 2 Login, 3 Go Back (`return false`). Failed login: forgot-password **1/0**. |
| `registraion` | Username + password + confirm; append two lines to `customer_account_save.txt`. Duplicate username → abort. |
| `login` | Username + `getPasswordFromUser()`; match adjacent lines in the save file. |
| `getPasswordFromUser` | `_getch` until Enter; echo `*`; Backspace supported. |
| `updateValueInFile` | Find username line, replace the **next** line (password) via `temp.txt`. |
| `show_customer_menu` | Customer view (below). |
| `display_Customer` | Commented out. |

Validation (register and forgot-password): length **6–10** for name/password, no spaces, confirm must match. Error text says “7 CHARACTERS MINIMUM” — the `if` uses `< 6`, so six characters are accepted. `cin >>` already stops at spaces.

Sample `customer_account_save.txt`:

```text
rohaan
123456
```

(plus other short lines in the committed file). Accounts are **plain text**, username then password, no hashing.

---

## Customer menus

`show_customer_menu` (“CUSTOMER VIEW”):

| Key | Label | Implementation |
|-----|--------|----------------|
| 1 | Place An Order | `items.print_Items_Menu()` |
| 2 | Modify An Item | `system("cls"); break;` — **no modify logic** |
| 3 | Display Cart Items | `items.Display_Cart()` → `Cart::DisplayItems` |
| 4 | Search An Item | **Printed only** — no `if (choice == 4)` |
| 5 | Remove An Item | `items.Item_Remove_from_Cart()` |
| 6 | Show Total Bill | `items.Bill()` |
| 7 | Go Back | `items.reset()` (`Cart::reset_data`) and `return false` |

Invalid integers: `cin.clear` + `cin.ignore(MAXDWORD, '\n')`. Nested failures `goto label` to redraw the customer menu.

`Cart::Search` is **declared** in `Cart.h` and **not defined** in `Cart.cpp`. It is never called (would fail to link if wired to option 4).

---

## Item catalogs and file I/O

`Item::print_Items_Menu`:

1. Read `ItemsCategory.txt` (one category name per line).
2. User picks an index, or **0** to go back.
3. Open `<category name>.txt`.
4. Parse lines: `getline(..., '-')` name, `getline(..., ',')` price, `getline >> ws` quantity.
5. User picks item #, then quantity `1 … stock`.
6. `c.add_item(name, price, qty)` and `update_items(filename, name, price, newQty)`.

Catalog line format (example `Groceries.txt`):

```text
Apples - 150 PKR, 2
Bananas - 80 PKR, 15
```

`update_items` rewrites the category file through `temp.txt`, matching **name and price**, writing `name-price, newQty`.

`Item_Remove_from_Cart` displays the cart then `Cart::remove_item`, which tries to **restock** by scanning every category file (implementation is brittle: one `temp.txt` for all categories, `remove(filename)` uses the last opened name without `.txt` in some paths).

---

## Cart

`Cart` (`Cart.h` / `Cart.cpp`): parallel arrays `itemnames[500]`, `Items_Price[500]`, `Items_Quantity[500]`; `static int cart_size` **starts at 1** (index 0 unused). `total_bill` for checkout.

| Method | Behaviour |
|--------|-----------|
| `add_item` | New row or add quantity if the same `itemnames` exists |
| `DisplayItems` | Table; running `pr` from digits in the price string × qty |
| `remove_item` | Index in range `[1, cart_size)`; shift arrays; attempt restock |
| `Bill` | Sum `stoi(numeric_part) * qty`; print **5% delivery** `(total_bill * 5) / 100` and total |
| `reset_data` | `memset(&cart_size, 0, sizeof(cart_size))` |

Prices are stored as strings such as `" 150 PKR"`; `Bill` / `DisplayItems` keep `isdigit` and `'.'` then `stoi`.

---

## Windows console styling

`Other_Fun.h` / customer/item code:

- Font: Consolas-style via `CONSOLE_FONT_INFOEX`, height **22**, `FW_BOLD`.
- `SetConsoleTextAttribute(handle, BACKGROUND_INTENSITY | n)` with `n` in `{0, 1, 3, 4, 5, 15}` (comments list 0–15 color numbers).
- Welcome uses attribute `| 3`; role picker `| 5`; errors `| 4`; success `| 15`.

Requires a **Windows** console. Not portable to Linux/`ncurses`.

---

## Security

- Passwords stored **in plaintext** in `customer_account_save.txt`.
- Forgot-password: anyone who knows the **username** can set a new password (`updateValueInFile` does not verify the old password).
- Login masks input with `*`; registration `cin >>` shows the password.
- No encryption, no salted hash, no account lockout.

Do not reuse these credentials elsewhere.

---

## File-by-file

| File | Role |
|------|------|
| `Main.cpp` | Maximize, role switch, Customer only |
| `Other_Fun.h` | `Welcome_Message`, `Select_Role` |
| `Role.h` | Abstract base |
| `Customer.h` / `Customer.cpp` | Auth + customer menu |
| `Items.h` / `Items.cpp` | Catalog UI, stock update, cart wrappers |
| `Cart.h` / `Cart.cpp` | In-memory cart + bill |
| `Admin.h` / `Admin.cpp` | Stub (`#include "Admin.h"` only in `.cpp`) |
| `Vendor.h` / `Vendor.cpp` | Stub |
| `Shopping_Portal.sln` / `.vcxproj` | VS project |
| `Classes.txt` | Unused class sketch |
| `OOP Project Description.pdf` | Assignment statement |
| `Final Project.zip` / `Final Try.zip` / `Shopping_Portal.zip` | Archives (may duplicate sources) |

---

## Data files

`ItemsCategory.txt` lists:

Groceries · Personal Care · Household Essentials · Baby Care · Health and Wellness · Electronics · Home Appliances · Clothing and Fashion · Home Decor and Furniture · Sports and Fitness · Office and School Supplies · Pet Supplies.

Matching stock files: `Groceries.txt`, `Electronics.txt`, … (same stem + `.txt`).

Also: `customer_account_save.txt`, `temp.txt` (scratch during rewrites).

Working directory **must** be the project root so `ItemsCategory.txt` and `*.txt` catalogs resolve.

---

## Build and run

Windows, `windows.h`, `conio.h`, `fstream`.

1. Open `Shopping_Portal.sln`.
2. Run from the folder that contains the category files.
3. Choose **3** (Customer) → register or login (`rohaan` / `123456` if using the committed save file) → **1** to order.

There is no CMake. `g++` would still need Win32 for colors and `_getch`.

---

## Limitations

- Admin/Vendor: headers empty; `Main.cpp` ignores choices 1–2.
- Menu **2 Modify** and **4 Search** are incomplete; `Search()` undeclared-body.
- `login()` may fall off without `return false` on failed credentials (undefined `bool`).
- `print_Items_Menu` / `Bill` / `Item_Remove_from_Cart` can miss a `return` on some paths.
- Restock-on-remove and `temp.txt` category rewrite are error-prone.
- Passwords in cleartext; forgot-password is username-only.
- `Classes.txt` does not match `Role` / `Customer` / `Item` / `Cart`.

**Extend:** implement Admin/Vendor stock CRUD; hash passwords; wire Search/Modify; one cart instance with defined returns.

---

## Author

**Mohammad Rohaan** — 22I-2327  
[https://github.com/rohaan2802](https://github.com/rohaan2802)
