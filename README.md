# Shopping_Portal

Console **OOP shopping portal** (“FAST Shopping Portal”) with role selection, customer registration/login, category-based catalog files, cart, search, and billing. Built as a Visual Studio C++ project.

## Overview

`Main.cpp` maximizes the console, shows a welcome banner, and calls `Select_Role()`:

1. Admin  
2. Vendor  
3. Customer  
4. Close window  

**Customer** is fully implemented (`Customer` + `Item` + `Cart`). **Admin** and **Vendor** headers/cpp files are present as stubs (includes only)—role menu entries exist, but those flows are not implemented in the committed sources.

Catalog data is file-driven: `ItemsCategory.txt` lists categories; each category has a matching `*.txt` inventory file.

## Features

### Customer
- Registration / login with username & password rules (length checks, confirm password)  
- Accounts appended to `customer_account_save.txt`  
- Forgot-password path updates password in the account file  
- Customer menu:
  1. Place an order (browse categories → items → quantities)  
  2. Modify an item (menu entry present)  
  3. Display cart  
  4. Search an item  
  5. Remove an item from cart  
  6. Show total bill  
  7. Go back  

### Catalog categories (`ItemsCategory.txt`)
Groceries, Personal Care, Household Essentials, Baby Care, Health and Wellness, Electronics, Home Appliances, Clothing and Fashion, Home Decor and Furniture, Sports and Fitness, Office and School Supplies, Pet Supplies — each with a corresponding `.txt` stock file.

### Cart (`Cart` class)
- Add / display / search / remove items  
- Running total bill  
- Inventory quantity updates when ordering (`update_items` / remove helpers on `Item`)

### OOP layout
- Abstract `Role` (`registraion`, `login`)  
- `Customer : protected Role`  
- `Item` + `Cart` collaboration  
- UI helpers in `Other_Fun.h` (welcome, role select, console colors)

## Tech stack

| Component | Technology |
|-----------|------------|
| Language | C++ |
| Platform | Windows console (`windows.h`, `conio.h`) |
| Persistence | Text files (`fstream`) |
| IDE | `Shopping_Portal.sln` |

## Project structure

```
Shopping_Portal/
├── Main.cpp
├── Role.h
├── Admin.h / Admin.cpp          # stubs
├── Vendor.h / Vendor.cpp        # stubs
├── Customer.h / Customer.cpp
├── Items.h / Items.cpp
├── Cart.h / Cart.cpp
├── Other_Fun.h
├── ItemsCategory.txt
├── Groceries.txt … Pet Supplies.txt   # per-category stock
├── customer_account_save.txt
├── Shopping_Portal.sln / .vcxproj
└── *.zip archives (Final Project, etc.)
```

## How to build / run

1. Open `Shopping_Portal.sln` in Visual Studio.  
2. Build (Windows).  
3. Run with working directory = project folder so category `.txt` files and `customer_account_save.txt` are found.

## Usage

1. Launch → welcome screen.  
2. Choose **3** for Customer.  
3. Register or log in.  
4. Place orders by picking a category number, then item numbers and quantities.  
5. View cart, search, remove lines, or print the bill.  
6. Option **4** on the role screen posts `WM_CLOSE` to exit the console window.

## How to extend / modify

- Implement `Admin` / `Vendor` (stock CRUD, approvals) behind menu options 1–2.  
- Add or rename categories in `ItemsCategory.txt` and add matching `CategoryName.txt` files.  
- Harden password handling (currently plain text in `customer_account_save.txt`).  
- Complete “Modify an item” (choice 2) if still a no-op in your branch.

## Author

**rohaan2802** (course folder references 22I-2327 Mohammad Rohaan) — [https://github.com/rohaan2802](https://github.com/rohaan2802)
