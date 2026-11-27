#include <cmath>
#include "Commerce.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cctype>
#include <windows.h>
#include "Cart.h"
#include "Other_Fun.h"

using namespace std;

namespace
{
bool namesMatch(const string &a, const string &b) { return trimCopy(a) == trimCopy(b); }

bool pricesMatch(const string &a, const string &b)
{
    return trimCopy(a) == trimCopy(b);
}

int findCatalogStock(const string &name, const string &price)
{
    try
    {
        int stock = -1, matches = 0;
        for (const auto &p : shop::loadCatalog("."))
            if (namesMatch(p.name, name) && p.price == shop::money(price))
            {
                stock = p.quantity;
                ++matches;
            }
        return matches == 1 ? stock : -1;
    }
    catch (...)
    {
        return -1;
    }
}

string cartHeader()
{
    /* 6+2+22+12+6+14 = CONTENT_WIDTH (62) */
    ostringstream hdr;
    hdr << fitField("ITEM#", 6, false) << "  " << fitField("ITEM NAME", 22, true)
        << fitField("UNIT PRICE", 12, true) << fitField("QTY", 6, true)
        << fitField("LINE TOTAL", 14, true);
    return hdr.str();
}

string cartRow(int idx, const string &name, double unit, int qty, double lineTotal)
{
    ostringstream row;
    row << paddedIndex(idx, 6) << "  " << fitField(trimCopy(name), 22, true)
        << fitField("Rs. " + shop::formatMoney(
                                 static_cast<shop::Cents>(std::llround(unit * 100))),
                    12, true)
        << fitField(to_string(qty), 6, true)
        << fitField("Rs. " + shop::formatMoney(static_cast<shop::Cents>(
                                 std::llround(lineTotal * 100))),
                    14, true);
    return row.str();
}

string billLine(const string &label, double amount)
{
    /* 46+16 = CONTENT_WIDTH (62) */
    ostringstream row;
    row << fitField(label, 46, true)
        << fitField("Rs. " + shop::formatMoney(
                                 static_cast<shop::Cents>(std::llround(amount * 100))),
                    16, false);
    return row.str();
}
} // namespace

Cart::Cart()
{
    cart_size = 0;
    total_bill = 0;
    for (int i = 0; i < 500; i++)
    {
        itemnames[i] = "";
        Items_Price[i] = "";
        Items_Quantity[i] = 0;
    }
}

int Cart::catalogStock(const string &itemName, const string &itemPrice)
{
    return findCatalogStock(itemName, itemPrice);
}

void Cart::add_item(string i_n, string i_p, int i_q)
{
    if (!shop::safeField(i_n))
    {
        errorMsg("Invalid product name");
        return;
    }
    try
    {
        shop::money(i_p);
    }
    catch (...)
    {
        errorMsg("Invalid price");
        return;
    }
    if (i_q <= 0 || i_q > 9999)
    {
        errorMsg("Invalid quantity");
        return;
    }
    int index = -1;
    for (int i = 0; i < cart_size; i++)
    {
        // Same product = same name AND same price (avoids merging cross-category
        // lookalikes)
        if (namesMatch(itemnames[i], i_n) && pricesMatch(Items_Price[i], i_p))
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        if (cart_size >= 500)
        {
            errorMsg("Cart is full");
            return;
        }
        itemnames[cart_size] = i_n;
        Items_Price[cart_size] = i_p;
        Items_Quantity[cart_size] = i_q;
        cart_size++;
    }
    else
    {
        if (Items_Quantity[index] > 9999 - i_q)
        {
            errorMsg("Quantity limit exceeded");
            return;
        }
        Items_Quantity[index] += i_q;
    }

    infoMsg("Item(s) Added to Cart Successfully!");
}

void Cart::reset_data()
{
    cart_size = 0;
    total_bill = 0;
    for (int i = 0; i < 500; i++)
    {
        itemnames[i] = "";
        Items_Price[i] = "";
        Items_Quantity[i] = 0;
    }
}

bool Cart::validateAndDeductStock(const std::string &orderRecord)
{
    try
    {
        if (cart_size == 0)
            return false;
        auto products = shop::loadCatalog(".");
        std::map<std::string, bool> touched;
        for (int i = 0; i < cart_size; ++i)
        {
            std::vector<shop::Product *> matches;
            for (auto &p : products)
                if (namesMatch(p.name, itemnames[i]) &&
                    p.price == shop::money(Items_Price[i]))
                    matches.push_back(&p);
            if (matches.size() != 1 || Items_Quantity[i] <= 0 ||
                matches[0]->quantity < Items_Quantity[i])
            {
                errorMsg("Unavailable, ambiguous or insufficient stock: " +
                         trimCopy(itemnames[i]));
                return false;
            }
            matches[0]->quantity -= Items_Quantity[i];
            touched[matches[0]->category] = true;
        }
        std::map<std::filesystem::path, std::string> updates;
        for (const auto &category : touched)
        {
            std::ostringstream out;
            for (const auto &p : products)
                if (p.category == category.first)
                    out << p.name << " - " << p.priceLabel << ", " << p.quantity
                        << "\n";
            updates[category.first + ".txt"] = out.str();
        }
        if (!orderRecord.empty())
        {
            auto previous = shop::readFile("orders.txt");
            if (!previous.empty() && previous.back() != '\n')
                previous += '\n';
            updates["orders.txt"] = previous + orderRecord + "\n";
        }
        shop::commitFiles(updates);
        return true;
    }
    catch (const std::exception &e)
    {
        errorMsg(std::string("Checkout failed: ") + e.what());
        return false;
    }
}

void Cart::DisplayItems()
{
    setDefaultColor();
    cout << "\n\n";
    if (cart_size <= 0)
    {
        centerPrint("<<<<<  CART IS EMPTY  >>>>>");
        cout << "\n";
        return;
    }

    centerPrint("<<<<<  DISPLAYING ITEMS IN CART  >>>>>");
    cout << "\n";
    contentPrint(cartHeader());
    cout << "\n";

    for (int i = 0; i < cart_size; i++)
    {
        string numeric = extractPriceNumber(Items_Price[i]);
        double unit = 0;
        parseDoubleSafe(numeric, unit);
        double line = unit * Items_Quantity[i];
        contentPrint(cartRow(i + 1, itemnames[i], unit, Items_Quantity[i], line));
    }
    cout << "\n\n";
}

void Cart::Search(const string &query)
{
    setDefaultColor();
    cout << "\n\n";
    contentPrint("Search results in cart for: \"" + query + "\"");
    cout << "\n";
    bool found = false;
    string q = query;
    for (char &c : q)
        c = static_cast<char>(tolower(static_cast<unsigned char>(c)));

    for (int i = 0; i < cart_size; i++)
    {
        string name = itemnames[i];
        string lower = name;
        for (char &c : lower)
            c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
        if (lower.find(q) != string::npos)
        {
            found = true;
            printNumberedLine(cout, i + 1,
                              trimCopy(itemnames[i]) + "  |  " +
                                  trimCopy(Items_Price[i]) +
                                  "  |  Qty: " + to_string(Items_Quantity[i]),
                              2);
        }
    }
    if (!found)
        infoMsg("No matching items in cart");
    cout << "\n";
}

bool Cart::modify_quantity()
{
    if (cart_size <= 0)
    {
        errorMsg("Cart is empty - nothing to modify");
        return false;
    }
    DisplayItems();
    int index = readIntInRange("\nChoose ITEM # to modify quantity (0 = cancel):   ", 0,
                               cart_size);
    if (index == 0)
        return false;

    index -= 1;
    clearScreen();
    sectionTitle("MODIFY QUANTITY", 6);
    contentPrint("Item: " + trimCopy(itemnames[index]));
    contentPrint("Price: " + trimCopy(Items_Price[index]));
    contentPrint("Current Qty: " + to_string(Items_Quantity[index]));
    cout << "\n";
    int newQty = readIntInRange("Enter new quantity (1 or more):   ", 1, 9999);

    int stock = findCatalogStock(itemnames[index], Items_Price[index]);
    if (stock < 0)
    {
        errorMsg("Could not locate product in catalog");
        return false;
    }
    if (newQty > stock)
    {
        errorMsg("Not enough stock. Available: " + to_string(stock));
        return false;
    }

    Items_Quantity[index] = newQty;
    successMsg("Quantity updated (stock deducts at checkout)");
    return true;
}

void Cart::remove_item()
{
    if (cart_size <= 0)
    {
        errorMsg("Cart is empty");
        return;
    }

    int index =
        readIntInRange("\n\nChoose item # to remove (0 = Back):   ", 0, cart_size);
    if (index == 0)
    {
        infoMsg("Going back...");
        return;
    }
    index -= 1; // 0-based

    for (int i = index; i < cart_size - 1; i++)
    {
        itemnames[i] = itemnames[i + 1];
        Items_Price[i] = Items_Price[i + 1];
        Items_Quantity[i] = Items_Quantity[i + 1];
    }
    cart_size--;
    itemnames[cart_size] = "";
    Items_Price[cart_size] = "";
    Items_Quantity[cart_size] = 0;

    infoMsg("Item(s) Removed Successfully");
}

bool Cart::Bill(double taxPercent, double deliveryPercent)
{
    total_bill = 0;
    if (cart_size <= 0)
    {
        errorMsg("Cart is empty - cannot generate bill");
        return false;
    }

    DisplayItems();

    shop::Cents subtotalCents = 0;
    for (int i = 0; i < cart_size; ++i)
        subtotalCents += shop::money(Items_Price[i]) * Items_Quantity[i];
    const auto totals =
        shop::calculate(subtotalCents, static_cast<int>(std::llround(taxPercent * 100)),
                        static_cast<int>(std::llround(deliveryPercent * 100)));
    const double subtotal = totals.subtotal / 100.0, tax = totals.tax / 100.0,
                 delivery = totals.delivery / 100.0;
    total_bill = totals.total / 100.0;

    string rule(static_cast<size_t>(CONTENT_WIDTH), '-');
    setColor(3);
    contentPrint(rule);
    contentPrint(billLine("SUBTOTAL", subtotal));
    {
        ostringstream lab;
        lab << "TAX (" << taxPercent << "%)";
        contentPrint(billLine(lab.str(), tax));
    }
    {
        ostringstream lab;
        lab << "Delivery Charges (" << deliveryPercent << "%)";
        contentPrint(billLine(lab.str(), delivery));
    }
    contentPrint(rule);
    setColor(15);
    contentPrint(billLine("TOTAL AMOUNT", total_bill));
    setDefaultColor();
    return true;
}
