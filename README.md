# Shopping_Portal (FAST Shopping Portal)

Windows **console OOP shop**: role picker, **Customer** register/login/cart/billing, file-based catalogs. **Admin** and **Vendor** headers exist as **stubs**.

**Team/course refs:** 22I-2327 Mohammad Rohaan · [rohaan2802](https://github.com/rohaan2802)

---

## Table of contents

1. [Entry and roles](#entry-and-roles)
2. [Customer flow](#customer-flow)
3. [Catalog files](#catalog-files)
4. [OOP map](#oop-map)
5. [Build](#build)
6. [Gaps](#gaps)

---

## Entry and roles

`Main.cpp` maximizes the console, welcome banner, `Select_Role()`:

1. Admin *(not implemented in committed cpp)*  
2. Vendor *(stub)*  
3. Customer *(full)*  
4. Close window (`WM_CLOSE`)

`Customer.cpp` (`_sources/Shopping_Customer.cpp`): `customer_Reg_Log_Menu` — Registration / Login / Go Back, colored `SetConsoleTextAttribute`, `goto label1` after failed nested menus. Invalid `cin` cleared with `MAXDWORD`. Failed login offers **forgot password** (0/1) updating `customer_account_save.txt`.

---

## Customer flow

Registration/login: username/password rules (length, confirm password). Accounts **append** to `customer_account_save.txt` (**plain text**).

After login, `show_customer_menu()`:

1. Place an order — categories → items → quantities (`update_items` on stock files)  
2. Modify an item *(menu entry; may be incomplete)*  
3. Display cart  
4. Search an item  
5. Remove from cart  
6. Show total bill  
7. Go back  

`Cart`: add / display / search / remove / running total. `Item` updates inventory files when ordering.

---

## Catalog files

`ItemsCategory.txt` lists categories; each has a matching **`CategoryName.txt`** stock file:

Groceries, Personal Care, Household Essentials, Baby Care, Health and Wellness, Electronics, Home Appliances, Clothing and Fashion, Home Decor and Furniture, Sports and Fitness, Office and School Supplies, Pet Supplies.

Working directory = project root so these files resolve.

---

## OOP map

- Abstract `Role`: `registraion`, `login` (typo in the method name is in source)  
- `Customer : protected Role`  
- `Item` + `Cart`  
- `Other_Fun.h` — welcome, role select, colors  
- `Admin.h/.cpp`, `Vendor.h/.cpp` — stubs (includes only)

Also: `Shopping_Items.cpp`, `Shopping_Cart.cpp`, `Shopping_Vendor.cpp`, `Shopping_Admin.cpp` under `_sources/` / the VS project.

---

## Build

Open `Shopping_Portal.sln` (Windows). `windows.h`, `conio.h`, `fstream`.

---

## Gaps

- Admin/Vendor stock CRUD not wired.  
- Passwords in cleartext.  
- Confirm menu option 2 (modify).  
- Zip archives (`Final Project`, etc.) may duplicate sources.

**Extend:** implement Admin/Vendor; hash passwords; add a category = new line in `ItemsCategory.txt` + matching `.txt`.

---

## Author

**rohaan2802** (22I-2327 Mohammad Rohaan) · [https://github.com/rohaan2802](https://github.com/rohaan2802)
