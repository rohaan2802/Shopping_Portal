#!/usr/bin/env python3
"""Generate documentation screenshots for FAST Shopping Portal (console mockups)."""
from __future__ import annotations

from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

OUT = Path(__file__).resolve().parents[1] / "docs" / "screenshots"
OUT.mkdir(parents=True, exist_ok=True)

TERM_W = 1100
PAD = 28
LINE_H = 26
HEADER_H = 44
BG = (12, 18, 28)
HEADER_BG = (24, 32, 48)
BORDER = (70, 90, 120)
TITLE_FG = (160, 180, 210)
FG = (230, 236, 245)
CYAN = (80, 220, 220)
MAGENTA = (220, 140, 220)
GREEN = (120, 230, 160)
YELLOW = (240, 210, 100)
RED = (255, 120, 120)
DIM = (140, 150, 170)
WHITE = (255, 255, 255)


def font(size=17, bold=False):
    candidates = [
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/consolab.ttf" if bold else "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/lucon.ttf",
        "DejaVuSansMono.ttf",
    ]
    if bold:
        candidates.insert(0, "C:/Windows/Fonts/consolab.ttf")
    for path in candidates:
        try:
            return ImageFont.truetype(path, size)
        except OSError:
            continue
    return ImageFont.load_default()


FONT = font(17)
FONT_B = font(18, bold=True)
FONT_SM = font(14)


def color_for(line: str):
    s = line.strip()
    if "ERROR" in s or "***" in s or "Invalid" in s:
        return RED
    if "SUCCESS" in s or "----" in s and ("LOGIN" in s or "REGISTER" in s or "ORDER" in s or "UPDATED" in s or "ADDED" in s):
        return WHITE
    if s.startswith("<<") or "<<<<<<<<" in s:
        return CYAN
    if "#####" in s or "****" in s or "WELCOME" in s or "DASHBOARD" in s:
        return CYAN
    if s[:2].isdigit() and ")" in s[:6]:
        return MAGENTA
    if s.startswith("1)") or s.startswith("2)") or s.startswith("3)") or s.startswith("4)") or s.startswith("5)") or s.startswith("6)") or s.startswith("7)") or s.startswith("8)") or s.startswith("9)"):
        return MAGENTA
    if "Rs." in s or "TOTAL" in s or "SUBTOTAL" in s or "TAX" in s or "Delivery" in s:
        return YELLOW
    if s.startswith("                         #") or "Order" in s:
        return GREEN
    if not s:
        return FG
    return FG


def render(ssid: str, heading: str, lines: list[str], filename: str | None = None):
    content_h = max(len(lines), 8) * LINE_H
    term_h = HEADER_H + content_h + PAD * 2 + 36
    img_w = TERM_W + 40
    img_h = term_h + 90
    img = Image.new("RGB", (img_w, img_h), (32, 36, 44))
    draw = ImageDraw.Draw(img)

    # Caption / heading above terminal
    draw.text((20, 14), f"{ssid}  —  {heading}", fill=WHITE, font=FONT_B)

    x0, y0 = 20, 48
    # shadow
    draw.rounded_rectangle([x0 + 5, y0 + 5, x0 + TERM_W + 5, y0 + term_h + 5], 12, fill=(0, 0, 0))
    draw.rounded_rectangle([x0, y0, x0 + TERM_W, y0 + term_h], 12, fill=BG, outline=BORDER, width=2)
    draw.rounded_rectangle([x0, y0, x0 + TERM_W, y0 + HEADER_H], 12, fill=HEADER_BG)
    draw.rectangle([x0, y0 + 20, x0 + TERM_W, y0 + HEADER_H], fill=HEADER_BG)

    # traffic lights
    for i, col in enumerate([(255, 95, 86), (255, 189, 46), (39, 201, 63)]):
        draw.ellipse([x0 + 16 + i * 22, y0 + 14, x0 + 30 + i * 22, y0 + 28], fill=col)

    draw.text((x0 + 100, y0 + 12), "FAST Shopping Portal — Windows Console", fill=TITLE_FG, font=FONT_SM)
    draw.text((x0 + TERM_W - 160, y0 + 12), "22I-2327 · Sec-Z", fill=DIM, font=FONT_SM)

    y = y0 + HEADER_H + 18
    for line in lines:
        draw.text((x0 + PAD, y), line[:110], fill=color_for(line), font=FONT)
        y += LINE_H

    out_name = filename or f"{ssid.lower()}.png"
    path = OUT / out_name
    img.save(path, "PNG")
    print("wrote", path.name)
    return path


SCREENS = [
    (
        "SS01",
        "Welcome Banner",
        [
            "",
            "    ************************************  WELCOME TO FAST SHOPPING PORTAL  ************************************",
            "",
            "                        Author: Mohammad Rohaan  |  22I-2327  |  Sec-Z",
            "                        OOP Console Shopping Portal — Admin · Vendor · Customer",
            "",
        ],
    ),
    (
        "SS02",
        "Role Selection Menu",
        [
            "",
            "                                              What Is Your Login Type....",
            "",
            "                                                    1. Admin",
            "",
            "                                                    2. Vendor",
            "",
            "                                                    3. Customer",
            "",
            "                                                    4. Exit / Close Window",
            "",
            "                           Enter Your Choice here                   _",
        ],
    ),
    (
        "SS03",
        "Admin Login Prompt",
        [
            "",
            "              ####################  ADMIN LOGIN  ####################",
            "",
            "                          Username:   admin",
            "                          Password:   ********",
            "",
        ],
    ),
    (
        "SS04",
        "Admin Login Success",
        [
            "",
            "                          ---------------  ADMIN LOGIN SUCCESSFUL  ---------------",
            "",
        ],
    ),
    (
        "SS05",
        "Admin Dashboard",
        [
            "",
            "    **********************************************************************************************",
            "                           ADMIN DASHBOARD — admin",
            "    **********************************************************************************************",
            "",
            "                          1)  View All Users",
            "",
            "                          2)  Manage Categories",
            "",
            "                          3)  Manage Products (CRUD)",
            "",
            "                          4)  View Orders & Stats",
            "",
            "                          5)  Create Another Admin",
            "",
            "                          6)  Logout",
        ],
    ),
    (
        "SS06",
        "Admin — View All Users",
        [
            "",
            "              ####################  VIEW ALL USERS  ####################",
            "",
            "                          === CUSTOMERS ===",
            "                          1)  Username: rohaan",
            "                          2)  Username: customer1",
            "",
            "                          === VENDORS ===",
            "                          1)  Username: vendor1  |  Company: FastGoods Co",
            "",
            "                          === ADMINS ===",
            "                          1)  Username: admin",
        ],
    ),
    (
        "SS07",
        "Admin — Manage Categories",
        [
            "",
            "              ####################  MANAGE CATEGORIES  ####################",
            "",
            "                          1)  Groceries",
            "                          2)  Personal Care",
            "                          3)  Electronics",
            "                          ...",
            "                          12) Pet Supplies",
            "",
            "                          1) Add Category",
            "                          2) Remove Category",
            "                          3) Back",
        ],
    ),
    (
        "SS08",
        "Admin — Add Category Success",
        [
            "",
            "                          New category name:   Outdoor Gear",
            "",
            "                          ---------------  Category added: Outdoor Gear  ---------------",
        ],
    ),
    (
        "SS09",
        "Admin — Manage Products CRUD",
        [
            "",
            "              ####################  MANAGE PRODUCTS (CRUD)  ####################",
            "",
            "                          1) View products in category",
            "                          2) Add product",
            "                          3) Update product",
            "                          4) Remove product",
            "                          5) Back",
        ],
    ),
    (
        "SS10",
        "Admin — View Category Products",
        [
            "",
            "                          Category name:   Groceries",
            "",
            "                          #         NAME                        PRICE               QTY",
            "                          1         Apples                      150 PKR             2",
            "                          2         Bananas                     80 PKR              15",
            "                          3         Bread                       120 PKR             5",
            "                          4         Milk                        180 PKR             8",
        ],
    ),
    (
        "SS11",
        "Admin — Orders & Statistics",
        [
            "",
            "              ####################  ORDERS & STATISTICS  ####################",
            "",
            "                          ID      CUSTOMER        TOTAL         DETAILS",
            "                          1001    rohaan          Rs. 450       Apples x2; Bread x1",
            "                          1002    rohaan          Rs. 890       Milk x2; Eggs x1",
            "",
            "                          Total Orders : 2",
            "                          Total Revenue: Rs. 1340",
        ],
    ),
    (
        "SS12",
        "Vendor Portal Gateway",
        [
            "",
            "    **********************************************************************************************",
            "                           VENDOR PORTAL",
            "    **********************************************************************************************",
            "",
            "                          1) Register",
            "",
            "                          2) Login",
            "",
            "                          3) Back",
        ],
    ),
    (
        "SS13",
        "Vendor Registration",
        [
            "",
            "              ####################  VENDOR REGISTRATION  ####################",
            "",
            "                          Username (6-16):   vendor1",
            "                          Password:   *******",
            "                          Confirm:    *******",
            "                          Company name:   FastGoods Co",
            "",
            "                          ---------------  VENDOR REGISTERED SUCCESSFULLY  ---------------",
        ],
    ),
    (
        "SS14",
        "Vendor Login Success",
        [
            "",
            "              ####################  VENDOR LOGIN  ####################",
            "",
            "                          Username:   vendor1",
            "                          Password:   *******",
            "",
            "                          ---------------  VENDOR LOGIN SUCCESSFUL — FastGoods Co  ---------------",
        ],
    ),
    (
        "SS15",
        "Vendor Dashboard",
        [
            "",
            "    **********************************************************************************************",
            "                    VENDOR DASHBOARD — vendor1 (FastGoods Co)",
            "    **********************************************************************************************",
            "",
            "                          1)  Manage My Products / Stock",
            "",
            "                          2)  View Catalog Categories",
            "",
            "                          3)  Search Catalog",
            "",
            "                          4)  Logout",
        ],
    ),
    (
        "SS16",
        "Vendor — Stock Management",
        [
            "",
            "              ####################  VENDOR STOCK MANAGEMENT — FastGoods Co  ####################",
            "",
            "                          1) View my products",
            "",
            "                          2) Add product to category",
            "",
            "                          3) Update my product stock/price",
            "",
            "                          4) Remove my product",
            "",
            "                          5) Back",
        ],
    ),
    (
        "SS17",
        "Vendor — My Products List",
        [
            "",
            "              ####################  MY PRODUCTS — FastGoods Co  ####################",
            "",
            "                          1) [Electronics] Demo Speaker  |  3500 PKR  |  Stock: 10",
        ],
    ),
    (
        "SS18",
        "Customer Gateway",
        [
            "",
            "    **********************************************************************************************",
            "                           CUSTOMER GATEWAY",
            "    **********************************************************************************************",
            "",
            "                                                   1)    Registration",
            "",
            "                                                   2)    Login",
            "",
            "                                                   3)    Forgot Password",
            "",
            "                                                   4)    Go Back",
        ],
    ),
    (
        "SS19",
        "Customer Registration",
        [
            "",
            "              ####################  CUSTOMER REGISTRATION  ####################",
            "",
            "                                               Enter Your User_Name                       customer1",
            "                                               Enter Your password                        ********",
            "                                               Enter Confirm_Password                     ********",
            "",
            "                          ---------------  REGISTERED SUCCESSFULLY  ---------------",
        ],
    ),
    (
        "SS20",
        "Customer Login (Masked Password)",
        [
            "",
            "              ####################  CUSTOMER LOGIN  ####################",
            "",
            "                                                     Enter Your User_Name                   rohaan",
            "                                                     Enter Your password                    ******",
            "",
            "                          ---------------  LOGIN SUCCESSFUL — Welcome rohaan  ---------------",
        ],
    ),
    (
        "SS21",
        "Forgot Password Flow",
        [
            "",
            "              ####################  FORGOT PASSWORD  ####################",
            "",
            "                                               Enter Your User_Name                         rohaan",
            "                                               Enter Your New_Password                      ******",
            "                                               Enter Confirm New_Password                   ******",
            "",
            "                          ---------------  PASSWORD UPDATED SUCCESSFULLY  ---------------",
        ],
    ),
    (
        "SS22",
        "Customer Main Menu",
        [
            "",
            "    **********************************************************************************************",
            "                           CUSTOMER VIEW — rohaan",
            "    **********************************************************************************************",
            "",
            "                                                 1)  Place An Order / Browse",
            "",
            "                                                 2)  Modify Cart Quantity",
            "",
            "                                                 3)  Display Cart Items",
            "",
            "                                                 4)  Search Products",
            "",
            "                                                 5)  Remove An Item From Cart",
            "",
            "                                                 6)  Show Bill / Checkout",
            "",
            "                                                 7)  Wishlist",
            "",
            "                                                 8)  Order History",
            "",
            "                                                 9)  Logout",
        ],
    ),
    (
        "SS23",
        "Browse Categories",
        [
            "",
            "              ####################  ORDER PLACING — BROWSE CATEGORIES  ####################",
            "",
            "                          Select a category (or 0 to go back)",
            "",
            "                                            01)  Groceries",
            "",
            "                                            02)  Personal Care",
            "",
            "                                            06)  Electronics",
            "",
            "                                            12)  Pet Supplies",
            "",
            "                          GO BACK = 0   |   Enter category #:   _",
        ],
    ),
    (
        "SS24",
        "Category Product Listing",
        [
            "",
            "              ####################  Groceries  ####################",
            "",
            "                          ITEM #    ITEM NAME                   PRICE               STOCK",
            "                          1         Apples                      150 PKR             2",
            "                          2         Bananas                     80 PKR              15",
            "                          3         Bread                       120 PKR             5",
            "                          4         Milk                        180 PKR             8",
            "                          5         Eggs                        200 PKR             12",
            "",
            "                          Add ITEM # to cart (0 = back to categories):   _",
        ],
    ),
    (
        "SS25",
        "Add to Cart Success",
        [
            "",
            "                          Enter quantity:   2",
            "",
            "                          <<<<<<<<<  Item(s) Added to Cart Successfully!  <<<<<<<<<",
        ],
    ),
    (
        "SS26",
        "Display Cart Items",
        [
            "",
            "              ####################  YOUR CART  ####################",
            "",
            "                          <<<<<<<<<<<<<<<<<<<  DISPLAYING ITEMS IN CART   <<<<<<<<<<<<<<<<<<<",
            "",
            "                          ITEM #    ITEM NAME                   UNIT PRICE    QTY    LINE TOTAL",
            "                          1         Apples                      Rs. 150       2      Rs. 300",
            "                          2         Bread                       Rs. 120       1      Rs. 120",
            "",
            "                          GO BACK, PRESS 0:   _",
        ],
    ),
    (
        "SS27",
        "Modify Cart Quantity",
        [
            "",
            "              ####################  MODIFY CART QUANTITY  ####################",
            "",
            "                          ITEM #    ITEM NAME                   UNIT PRICE    QTY    LINE TOTAL",
            "                          1         Apples                      Rs. 150       2      Rs. 300",
            "",
            "                          Choose ITEM # to modify quantity (0 = cancel):   1",
            "                          Enter new quantity (1 or more):   1",
            "",
            "                          ---------------  Quantity updated  ---------------",
        ],
    ),
    (
        "SS28",
        "Search Products Across Catalog",
        [
            "",
            "              ####################  SEARCH PRODUCTS  ####################",
            "",
            "                          Enter search keyword:   milk",
            "",
            "                          [Groceries]  Milk  |  180 PKR  | Stock: 8",
            "",
            "              ####################  SEARCH IN CART  ####################",
            "",
            "                          Search results in cart for: \"milk\"",
            "                          <<<<<<<<<  No matching items in cart  <<<<<<<<<",
        ],
    ),
    (
        "SS29",
        "Remove Item From Cart",
        [
            "",
            "              ####################  REMOVE FROM CART  ####################",
            "",
            "                          ITEM #    ITEM NAME                   UNIT PRICE    QTY    LINE TOTAL",
            "                          1         Apples                      Rs. 150       1      Rs. 150",
            "                          2         Bread                       Rs. 120       1      Rs. 120",
            "",
            "                          Choose The item_# To Remove From Cart:   2",
            "",
            "                          <<<<<<<<<  Item(s) Removed Successfully (stock restored)  <<<<<<<<<",
        ],
    ),
    (
        "SS30",
        "Bill With Tax and Delivery",
        [
            "",
            "              ####################  TOTAL BILL (Tax 5% + Delivery 5%)  ####################",
            "",
            "                          ITEM #    ITEM NAME                   UNIT PRICE    QTY    LINE TOTAL",
            "                          1         Apples                      Rs. 150       1      Rs. 150",
            "",
            "                                                                 ----------------------------",
            "   <<<<<<<< SUBTOTAL                                                              Rs. 150",
            "   <<<<<<<< TAX (5%)                                                              Rs. 7",
            "   <<<<<<<< Delivery Charges (5%)                                                 Rs. 7",
            "                                                                 ----------------------------",
            "   <<<<<<<< TOTAL AMOUNT                                                          Rs. 164",
        ],
    ),
    (
        "SS31",
        "Checkout — Order Placed",
        [
            "",
            "                          Place order? 1 = YES, 0 = NO:   1",
            "",
            "                          ---------------  ORDER PLACED — ID #1003  ---------------",
        ],
    ),
    (
        "SS32",
        "Wishlist Feature",
        [
            "",
            "              ####################  WISHLIST — rohaan  ####################",
            "",
            "                          1)  Wireless Earbuds",
            "                          2)  Yoga Mat",
            "",
            "                          1) Add item name to wishlist",
            "                          2) Remove from wishlist",
            "                          3) View wishlist",
            "                          4) Back",
            "",
            "                          (saved to wishlist.txt)",
        ],
    ),
    (
        "SS33",
        "Order History",
        [
            "",
            "              ####################  ORDER HISTORY — rohaan  ####################",
            "",
            "                          #1001  |  Total: Rs. 450  |  Apples x2; Bread x1 @ 2026-09-14 18:00",
            "",
            "                          #1002  |  Total: Rs. 890  |  Milk x2; Eggs x1 @ 2026-09-14 18:30",
            "",
            "                          #1003  |  Total: Rs. 164  |  Apples x1 @ 2026-09-14 19:00",
        ],
    ),
    (
        "SS34",
        "Input Validation Error",
        [
            "",
            "                          Enter Your Choice here                   abc",
            "                          Invalid input. Enter an integer in range [1 - 9]:   _",
            "",
            "                          *** USERNAME/PASSWORD 6-16 chars, no spaces, confirm must match ***",
        ],
    ),
    (
        "SS35",
        "Logout and Exit",
        [
            "",
            "                          ---------------  Logged out — cart cleared  ---------------",
            "",
            "                          Thank you for visiting FAST Shopping Portal!",
            "",
            "                          (Window close / choice 4 from role menu)",
        ],
    ),
]


def main():
    paths = []
    for ssid, heading, lines in SCREENS:
        paths.append(render(ssid, heading, lines, f"{ssid.lower()}.png"))
    print(f"\nGenerated {len(paths)} screenshots in {OUT}")


if __name__ == "__main__":
    main()
