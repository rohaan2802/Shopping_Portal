#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cctype>
#include <windows.h>
#include "Cart.h"
#include "Other_Fun.h"

using namespace std;

int Cart::cart_size = 0;

Cart::Cart()
{
	total_bill = 0;
	for (int i = 0; i < 500; i++)
	{
		itemnames[i] = "";
		Items_Price[i] = "";
		Items_Quantity[i] = 0;
	}
}

void Cart::add_item(string i_n, string i_p, int i_q)
{
	int index = -1;
	for (int i = 0; i < cart_size; i++)
	{
		if (itemnames[i] == i_n)
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

void Cart::DisplayItems()
{
	setColor(0);
	cout << "\n\n";
	if (cart_size <= 0)
	{
		cout << "                                           <<<<<<<<<<<<<<<<<<<  CART IS EMPTY   <<<<<<<<<<<<<<<<<<<\n\n";
		return;
	}

	cout << "                                           <<<<<<<<<<<<<<<<<<<  DISPLAYING ITEMS IN CART   <<<<<<<<<<<<<<<<<<<\n\n";
	cout << left;
	cout << "                         " << setw(10) << "ITEM #"
		<< setw(28) << "ITEM NAME"
		<< setw(18) << "UNIT PRICE"
		<< setw(12) << "QTY"
		<< setw(16) << "LINE TOTAL" << "\n\n";

	for (int i = 0; i < cart_size; i++)
	{
		string numeric = extractPriceNumber(Items_Price[i]);
		double unit = stod(numeric);
		double line = unit * Items_Quantity[i];

		cout << "                         " << setw(10) << (i + 1)
			<< setw(28) << itemnames[i]
			<< "Rs. " << setw(14) << (int)unit
			<< setw(12) << Items_Quantity[i]
			<< "Rs. " << setw(12) << (int)line << "\n";
	}
	cout << "\n\n";
}

void Cart::Search(const string& query)
{
	setColor(0);
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
			cout << "                         " << (i + 1) << ") " << itemnames[i]
				<< "  |  " << Items_Price[i] << "  |  Qty: " << Items_Quantity[i] << "\n";
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
		errorMsg("Cart is empty — nothing to modify");
		return false;
	}
	DisplayItems();
	int index = readIntInRange("\n                         Choose ITEM # to modify quantity (0 = cancel):   ", 0, cart_size);
	if (index == 0)
		return false;

	index -= 1;
	int newQty = readIntInRange("                         Enter new quantity (1 or more):   ", 1, 9999);
	int oldQty = Items_Quantity[index];
	Items_Quantity[index] = newQty;

	// Restock / deduct difference in catalog files
	int delta = oldQty - newQty; // positive => return to stock
	if (delta != 0)
	{
		ifstream cats("ItemsCategory.txt");
		string cat;
		bool updated = false;
		while (getline(cats, cat))
		{
			if (cat.empty()) continue;
			string path = cat + ".txt";
			ifstream in(path);
			if (!in.is_open()) continue;

			ofstream temp("temp.txt");
			string n, p, q;
			while (getline(in, n, '-') && getline(in, p, ',') && getline(in >> ws, q))
			{
				if (n == itemnames[index] && p == Items_Price[index])
				{
					int stock = stoi(q) + delta;
					if (stock < 0) stock = 0;
					temp << n << '-' << p << ", " << stock;
					updated = true;
				}
				else
				{
					temp << n << '-' << p << ", " << q;
				}
				if (!in.eof()) temp << "\n";
			}
			in.close();
			temp.close();
			if (updated)
			{
				remove(path.c_str());
				rename("temp.txt", path.c_str());
				break;
			}
			remove("temp.txt");
		}
	}

	successMsg("Quantity updated");
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

	// Restock into the correct category file
	ifstream cats("ItemsCategory.txt");
	string cat;
	bool restocked = false;
	while (getline(cats, cat))
	{
		if (cat.empty()) continue;
		string path = cat + ".txt";
		ifstream in(path);
		if (!in.is_open()) continue;

		ofstream temp("temp.txt");
		string n, p, q;
		bool touched = false;
		while (getline(in, n, '-') && getline(in, p, ',') && getline(in >> ws, q))
		{
			if (n == itemnames[index] && p == Items_Price[index])
			{
				int stock = stoi(q) + Items_Quantity[index];
				temp << n << '-' << p << ", " << stock;
				touched = true;
			}
			else
			{
				temp << n << '-' << p << ", " << q;
			}
			if (!in.eof()) temp << "\n";
		}
		in.close();
		temp.close();
		if (touched)
		{
			remove(path.c_str());
			rename("temp.txt", path.c_str());
			restocked = true;
			break;
		}
		remove("temp.txt");
	}
	cats.close();

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

	infoMsg(restocked ? "Item(s) Removed Successfully (stock restored)" : "Item(s) Removed Successfully");
}

bool Cart::Bill(double taxPercent, double deliveryPercent)
{
	total_bill = 0;
	if (cart_size <= 0)
	{
		errorMsg("Cart is empty — cannot generate bill");
		return false;
	}

	DisplayItems();

	double subtotal = 0;
	for (int i = 0; i < cart_size; i++)
	{
		string numeric = extractPriceNumber(Items_Price[i]);
		subtotal += stod(numeric) * Items_Quantity[i];
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
	setColor(0);
	return true;
}
