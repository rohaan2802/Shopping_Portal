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
	bool namesMatch(const string& a, const string& b)
	{
		return trimCopy(a) == trimCopy(b);
	}

	bool pricesMatch(const string& a, const string& b)
	{
		return trimCopy(a) == trimCopy(b);
	}

	// Adjust stock for one catalog line matching name+price. Returns true if a file was updated.
	bool adjustCatalogStock(const string& itemName, const string& itemPrice, int delta)
	{
		ifstream cats("ItemsCategory.txt");
		if (!cats.is_open())
			return false;

		string cat;
		while (getline(cats, cat))
		{
			if (trimCopy(cat).empty()) continue;
			cat = trimCopy(cat);
			string path = cat + ".txt";
			ifstream in(path);
			if (!in.is_open()) continue;

			ofstream temp("temp.txt");
			string n, p, q;
			bool touched = false;
			bool first = true;
			while (getline(in, n, '-') && getline(in, p, ',') && getline(in >> ws, q))
			{
				if (!first) temp << "\n";
				first = false;

				if (namesMatch(n, itemName) && pricesMatch(p, itemPrice))
				{
					int stock = 0;
					parseIntSafe(q, stock);
					stock += delta;
					if (stock < 0) stock = 0;
					temp << n << '-' << p << ", " << stock;
					touched = true;
				}
				else
				{
					temp << n << '-' << p << ", " << q;
				}
			}
			in.close();
			temp.close();

			if (touched)
			{
				remove(path.c_str());
				rename("temp.txt", path.c_str());
				cats.close();
				return true;
			}
			remove("temp.txt");
		}
		cats.close();
		return false;
	}

	int findCatalogStock(const string& itemName, const string& itemPrice)
	{
		ifstream cats("ItemsCategory.txt");
		if (!cats.is_open())
			return -1;

		string cat;
		while (getline(cats, cat))
		{
			if (trimCopy(cat).empty()) continue;
			cat = trimCopy(cat);
			ifstream in(cat + ".txt");
			if (!in.is_open()) continue;
			string n, p, q;
			while (getline(in, n, '-') && getline(in, p, ',') && getline(in >> ws, q))
			{
				if (namesMatch(n, itemName) && pricesMatch(p, itemPrice))
				{
					int stock = 0;
					if (!parseIntSafe(q, stock)) return 0;
					return stock;
				}
			}
		}
		return -1;
	}
}

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

int Cart::catalogStock(const string& itemName, const string& itemPrice)
{
	return findCatalogStock(itemName, itemPrice);
}

void Cart::add_item(string i_n, string i_p, int i_q)
{
	int index = -1;
	for (int i = 0; i < cart_size; i++)
	{
		// Same product = same name AND same price (avoids merging cross-category lookalikes)
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

bool Cart::validateAndDeductStock()
{
	// Pass 1: ensure every line is still available (stock never reserved until checkout)
	for (int i = 0; i < cart_size; i++)
	{
		int stock = findCatalogStock(itemnames[i], Items_Price[i]);
		if (stock < 0)
		{
			errorMsg("Product not found in catalog: " + trimCopy(itemnames[i]));
			return false;
		}
		if (stock < Items_Quantity[i])
		{
			errorMsg("Not enough stock for " + trimCopy(itemnames[i])
				+ " (need " + to_string(Items_Quantity[i])
				+ ", available " + to_string(stock) + ")");
			return false;
		}
	}

	// Pass 2: deduct once per line
	for (int i = 0; i < cart_size; i++)
	{
		if (!adjustCatalogStock(itemnames[i], Items_Price[i], -Items_Quantity[i]))
		{
			errorMsg("Failed to update stock for " + trimCopy(itemnames[i]));
			return false;
		}
	}
	return true;
}

void Cart::DisplayItems()
{
	setDefaultColor();
	cout << "\n\n";
	if (cart_size <= 0)
	{
		cout << "                                           <<<<<<<<<<<<<<<<<<<  CART IS EMPTY   <<<<<<<<<<<<<<<<<<<\n\n";
		return;
	}

	cout << "                                           <<<<<<<<<<<<<<<<<<<  DISPLAYING ITEMS IN CART   <<<<<<<<<<<<<<<<<<<\n\n";
	cout << "                         "
		<< right << setw(6) << "ITEM#"
		<< "  " << left << setw(28) << "ITEM NAME"
		<< setw(18) << "UNIT PRICE"
		<< setw(12) << "QTY"
		<< setw(16) << "LINE TOTAL" << "\n\n";

	for (int i = 0; i < cart_size; i++)
	{
		string numeric = extractPriceNumber(Items_Price[i]);
		double unit = 0;
		parseDoubleSafe(numeric, unit);
		double line = unit * Items_Quantity[i];

		cout << "                         ";
		printPaddedIndex(cout, i + 1, 6);
		cout << "  " << left << setw(28) << trimCopy(itemnames[i])
			<< "Rs. " << setw(14) << (int)unit
			<< setw(12) << Items_Quantity[i]
			<< "Rs. " << setw(12) << (int)line << "\n";
	}
	cout << "\n\n";
}

void Cart::Search(const string& query)
{
	setDefaultColor();
	cout << "\n\n                         Search results in cart for: \"" << query << "\"\n\n";
	bool found = false;
	string q = query;
	for (char& c : q) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));

	for (int i = 0; i < cart_size; i++)
	{
		string name = itemnames[i];
		string lower = name;
		for (char& c : lower) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
		if (lower.find(q) != string::npos)
		{
			found = true;
			printNumberedLine(cout, i + 1,
				trimCopy(itemnames[i]) + "  |  " + trimCopy(Items_Price[i])
				+ "  |  Qty: " + to_string(Items_Quantity[i]), 2);
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
	int index = readIntInRange("\n                         Choose ITEM # to modify quantity (0 = cancel):   ", 0, cart_size);
	if (index == 0)
		return false;

	index -= 1;
	int newQty = readIntInRange("                         Enter new quantity (1 or more):   ", 1, 9999);

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

	int index = readIntInRange("\n\n                             Choose The item_# To Remove From Cart:   ", 1, cart_size);
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

	double subtotal = 0;
	for (int i = 0; i < cart_size; i++)
	{
		string numeric = extractPriceNumber(Items_Price[i]);
		double unit = 0;
		parseDoubleSafe(numeric, unit);
		subtotal += unit * Items_Quantity[i];
	}

	double tax = (subtotal * taxPercent) / 100.0;
	double delivery = (subtotal * deliveryPercent) / 100.0;
	total_bill = subtotal + tax + delivery;

	setColor(3);
	cout << "                                                                                            -----------------------------------\n";
	cout << "\n  <<<<<<<< SUBTOTAL                                                                              Rs. " << (int)subtotal << endl;
	cout << "\n  <<<<<<<< TAX (" << taxPercent << "%)                                                                          Rs. " << (int)tax << endl;
	cout << "\n  <<<<<<<< Delivery Charges (" << deliveryPercent << "%)                                                           Rs. " << (int)delivery << endl;
	cout << "                                                                                            -----------------------------------\n";
	setColor(15);
	cout << "\n  <<<<<<<< TOTAL AMOUNT                                                                          Rs. " << (int)total_bill << endl;
	setDefaultColor();
	return true;
}
