#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <windows.h>
#include "Cart.h"
#include "Items.h"
#include "Other_Fun.h"

using namespace std;

Cart c;

namespace
{
	string productHeader()
	{
		/* 6+2+26+18+10 = CONTENT_WIDTH (62) */
		ostringstream hdr;
		hdr << fitField("ITEM#", 6, false) << "  "
			<< fitField("ITEM NAME", 26, true)
			<< fitField("PRICE", 18, true)
			<< fitField("STOCK", 10, true);
		return hdr.str();
	}

	string productRow(int idx, const string& name, const string& price, const string& stock)
	{
		ostringstream row;
		row << paddedIndex(idx, 6) << "  "
			<< fitField(trimCopy(name), 26, true)
			<< fitField(trimCopy(price), 18, true)
			<< fitField(trimCopy(stock), 10, true);
		return row.str();
	}
}

bool Item::print_Items_Menu()
{
	do
	{
		Item items[500];
		int arrayofcount[100] = { 0 };
		int i_q, itemref;
		ifstream read1;
		string filename;
		string Item_Category[100];
		int itemcount = 1, i = 1, itemcount1 = 1;
		string str;

	label:
		clearScreen();
		sectionTitle("ORDER PLACING - BROWSE CATEGORIES", 5);
		setDefaultColor();
		contentPrint("Select a category (or 0 to go back)");
		cout << "\n";

		ifstream read("ItemsCategory.txt");
		if (!read.is_open())
		{
			errorMsg("ItemsCategory.txt NOT FOUND");
			return false;
		}
		itemcount = 1;
		i = 1;
		while (getline(read, str))
		{
			if (str.empty()) continue;
			Item_Category[itemcount] = str;
			printNumberedLine(cout, itemcount, str, 2);
			cout << "\n";
			arrayofcount[i++] = itemcount++;
		}
		read.close();

		int choice = readIntInRange("\nGO BACK = 0   |   Enter category #:   ", 0, itemcount - 1);
		if (choice == 0)
			return false;

		bool b = false;
		for (int k = 1; k < itemcount; k++)
		{
			if (choice == arrayofcount[k])
			{
				b = true;
				filename = Item_Category[k];
				read1.open(filename + ".txt");
				if (!read1.is_open())
				{
					errorMsg("FILE NOT FOUND: " + filename + ".txt");
					goto label;
				}
				break;
			}
		}
		if (!b)
		{
			errorMsg("Invalid category");
			goto label;
		}

		clearScreen();
		sectionTitle(filename, 3);
		contentPrint(productHeader());
		cout << "\n";

		itemcount1 = 1;
		while (getline(read1, items[itemcount1].itemnames, '-') &&
			getline(read1, items[itemcount1].Items_Price, ',') &&
			getline(read1 >> ws, items[itemcount1].Items_Quantity))
		{
			// Skip duplicate name+price rows inside the same category file
			bool dup = false;
			for (int d = 1; d < itemcount1; d++)
			{
				if (trimCopy(items[d].itemnames) == trimCopy(items[itemcount1].itemnames) &&
					trimCopy(items[d].Items_Price) == trimCopy(items[itemcount1].Items_Price))
				{
					dup = true;
					break;
				}
			}
			if (dup) continue;

			int qty = 0;
			if (!parseIntSafe(items[itemcount1].Items_Quantity, qty))
				qty = 0;
			contentPrint(productRow(itemcount1, items[itemcount1].itemnames,
				items[itemcount1].Items_Price, to_string(qty)));
			itemcount1++;
			if (itemcount1 >= 500) break;
		}
		read1.close();

		do
		{
			itemref = readIntInRange("\nAdd ITEM # to cart (0 = back to categories):   ", 0, itemcount1 - 1);
			if (itemref == 0)
			{
				clearScreen();
				break;
			}

			int avail = 0;
			if (!parseIntSafe(items[itemref].Items_Quantity, avail) || avail <= 0)
			{
				infoMsg("Item Is Not Available");
				continue;
			}

			// Stock is not reserved until checkout - cap by catalog qty minus cart qty already held
			int alreadyInCart = 0;
			for (int ci = 0; ci < c.size(); ci++)
			{
				if (trimCopy(c.getName(ci)) == trimCopy(items[itemref].itemnames) &&
					trimCopy(c.getPrice(ci)) == trimCopy(items[itemref].Items_Price))
				{
					alreadyInCart = c.getQty(ci);
					break;
				}
			}
			int remaining = avail - alreadyInCart;
			if (remaining <= 0)
			{
				infoMsg("All available stock for this item is already in your cart");
				continue;
			}

			i_q = readIntInRange("Enter quantity:   ", 1, remaining);
			c.add_item(items[itemref].itemnames, items[itemref].Items_Price, i_q);
			// Catalog files unchanged until checkout
		} while (true);

	} while (true);
}

void Item::update_items(const string& filename, const string& name, string price, const string& newValue)
{
	string file_name = filename + ".txt";
	ifstream inputFile(file_name);
	if (!inputFile)
	{
		errorMsg("Unable to open " + file_name);
		return;
	}

	ofstream tempFile("temp.txt");
	Item items[1000];
	int itemcount1 = 0;
	bool first = true;

	while (getline(inputFile, items[itemcount1].itemnames, '-') &&
		getline(inputFile, items[itemcount1].Items_Price, ',') &&
		getline(inputFile >> ws, items[itemcount1].Items_Quantity))
	{
		if (!first) tempFile << "\n";
		first = false;

		if (trimCopy(items[itemcount1].itemnames) == trimCopy(name) &&
			trimCopy(items[itemcount1].Items_Price) == trimCopy(price))
			tempFile << items[itemcount1].itemnames << '-' << items[itemcount1].Items_Price << ", " << newValue;
		else
			tempFile << items[itemcount1].itemnames << '-' << items[itemcount1].Items_Price << ", " << items[itemcount1].Items_Quantity;

		itemcount1++;
	}

	inputFile.close();
	tempFile.close();
	remove(file_name.c_str());
	rename("temp.txt", file_name.c_str());
}

void Item::reset()
{
	c.reset_data(); // clear cart only - stock was never deducted early
}

bool Item::Display_Cart()
{
	clearScreen();
	sectionTitle("YOUR CART", 5);
	c.DisplayItems();
	readIntInRange("\nGO BACK, PRESS 0:   ", 0, 0);
	return false;
}

bool Item::Item_Remove_from_Cart()
{
	clearScreen();
	sectionTitle("REMOVE FROM CART", 4);
	c.DisplayItems();
	if (!c.isEmpty())
		c.remove_item();
	readIntInRange("\nGO BACK, PRESS 0:   ", 0, 0);
	return false;
}

bool Item::Modify_Cart_Item()
{
	clearScreen();
	sectionTitle("MODIFY CART QUANTITY", 6);
	c.modify_quantity();
	readIntInRange("\nGO BACK, PRESS 0:   ", 0, 0);
	return false;
}

bool Item::Search_Catalog()
{
	clearScreen();
	sectionTitle("SEARCH PRODUCTS", 3);
	string query = readLine("Enter search keyword:   ");
	if (query.empty())
	{
		errorMsg("Empty search");
		return false;
	}
	searchAllCatalogs(query);
	cout << "\n";
	sectionTitle("SEARCH IN CART", 5);
	c.Search(query);
	readIntInRange("\nGO BACK, PRESS 0:   ", 0, 0);
	return false;
}

bool Item::Bill()
{
	clearScreen();
	sectionTitle("TOTAL BILL (Tax 5% + Delivery 5%)", 3);
	c.Bill(5.0, 5.0);
	readIntInRange("\nGO BACK, PRESS 0:   ", 0, 0);
	return false;
}

bool Item::listCategories()
{
	ifstream read("ItemsCategory.txt");
	if (!read.is_open())
	{
		errorMsg("ItemsCategory.txt not found");
		return false;
	}
	string str;
	int n = 1;
	setDefaultColor();
	cout << "\n";
	while (getline(read, str))
	{
		if (str.empty()) continue;
		printNumberedLine(cout, n++, str, 2);
	}
	return true;
}

bool Item::addCategory(const string& name)
{
	string target = trimCopy(name);
	if (target.empty()) return false;
	ifstream check("ItemsCategory.txt");
	string line;
	bool emptyFile = true;
	while (getline(check, line))
	{
		if (trimCopy(line).empty()) continue;
		emptyFile = false;
		if (trimCopy(line) == target)
		{
			errorMsg("Category already exists");
			return false;
		}
	}
	check.close();

	ofstream out("ItemsCategory.txt", ios::app);
	if (!emptyFile) out << "\n";
	out << target;
	out.close();

	ofstream cat(target + ".txt", ios::app);
	cat.close();
	successMsg("Category added: " + target);
	return true;
}

bool Item::removeCategory(const string& name)
{
	ifstream in("ItemsCategory.txt");
	ofstream temp("temp.txt");
	string line;
	bool found = false;
	bool first = true;
	while (getline(in, line))
	{
		if (line.empty()) continue;
		if (line == name)
		{
			found = true;
			continue;
		}
		if (!first) temp << "\n";
		temp << line;
		first = false;
	}
	in.close();
	temp.close();
	if (!found)
	{
		remove("temp.txt");
		errorMsg("Category not found");
		return false;
	}
	remove("ItemsCategory.txt");
	rename("temp.txt", "ItemsCategory.txt");
	remove((name + ".txt").c_str());
	successMsg("Category removed: " + name);
	return true;
}

bool Item::addProduct(const string& category, const string& name, const string& priceLabel, int qty)
{
	string path = category + ".txt";
	string target = trimCopy(name);
	if (target.empty())
	{
		errorMsg("Product name required");
		return false;
	}

	bool empty = true;
	{
		ifstream check(path);
		if (check.is_open())
		{
			check.seekg(0, ios::end);
			empty = (check.tellg() == 0);
			check.clear();
			check.seekg(0, ios::beg);
			string n, p, q;
			while (getline(check, n, '-') && getline(check, p, ',') && getline(check >> ws, q))
			{
				if (trimCopy(n) == target)
				{
					errorMsg("Product already exists in this category");
					return false;
				}
			}
		}
	}
	ofstream out(path, ios::app);
	if (!out.is_open())
	{
		errorMsg("Cannot open category file");
		return false;
	}
	if (!empty) out << "\n";
	out << target << " - " << trimCopy(priceLabel) << ", " << qty;
	out.close();
	successMsg("Product added");
	return true;
}

bool Item::updateProduct(const string& category, const string& name, const string& newPriceLabel, int newQty)
{
	string path = category + ".txt";
	ifstream in(path);
	if (!in.is_open())
	{
		errorMsg("Category file missing");
		return false;
	}
	ofstream temp("temp.txt");
	string n, p, q;
	bool found = false;
	bool first = true;
	string target = trimCopy(name);
	while (getline(in, n, '-') && getline(in, p, ',') && getline(in >> ws, q))
	{
		if (!first) temp << "\n";
		first = false;

		if (trimCopy(n) == target)
		{
			found = true;
			temp << target << " - " << trimCopy(newPriceLabel) << ", " << newQty;
		}
		else
		{
			temp << n << '-' << p << ", " << q;
		}
	}
	in.close();
	temp.close();
	if (!found)
	{
		remove("temp.txt");
		errorMsg("Product not found");
		return false;
	}
	remove(path.c_str());
	rename("temp.txt", path.c_str());
	successMsg("Product updated");
	return true;
}

bool Item::removeProduct(const string& category, const string& name)
{
	string path = category + ".txt";
	ifstream in(path);
	if (!in.is_open())
	{
		errorMsg("Category file missing");
		return false;
	}
	ofstream temp("temp.txt");
	string n, p, q;
	bool found = false;
	bool first = true;
	string target = trimCopy(name);
	while (getline(in, n, '-') && getline(in, p, ',') && getline(in >> ws, q))
	{
		if (trimCopy(n) == target)
		{
			found = true;
			continue;
		}
		if (!first) temp << "\n";
		first = false;
		temp << n << '-' << p << ", " << q;
	}
	in.close();
	temp.close();
	if (!found)
	{
		remove("temp.txt");
		errorMsg("Product not found");
		return false;
	}
	remove(path.c_str());
	rename("temp.txt", path.c_str());
	successMsg("Product removed");
	return true;
}

bool Item::viewCategoryProducts(const string& category)
{
	ifstream in(category + ".txt");
	if (!in.is_open())
	{
		errorMsg("Cannot open " + category + ".txt");
		return false;
	}
	setDefaultColor();
	cout << "\n";
	{
		ostringstream hdr;
		hdr << fitField("#", 6, false) << "  "
			<< fitField("NAME", 26, true)
			<< fitField("PRICE", 18, true)
			<< fitField("QTY", 10, true);
		contentPrint(hdr.str());
		cout << "\n";
	}
	int i = 1;
	string n, p, q;
	while (getline(in, n, '-') && getline(in, p, ',') && getline(in >> ws, q))
	{
		contentPrint(productRow(i++, n, p, q));
	}
	return true;
}

bool Item::searchAllCatalogs(const string& query)
{
	ifstream cats("ItemsCategory.txt");
	string cat;
	string q = trimCopy(query);
	for (char& ch : q) ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
	bool any = false;
	setDefaultColor();
	// Deduplicate identical cat|name|price hits (e.g. duplicated file lines)
	string seenKeys[2000];
	int seenCount = 0;

	while (getline(cats, cat))
	{
		cat = trimCopy(cat);
		if (cat.empty()) continue;
		ifstream in(cat + ".txt");
		if (!in.is_open()) continue;
		string n, p, qty;
		while (getline(in, n, '-') && getline(in, p, ',') && getline(in >> ws, qty))
		{
			string name = trimCopy(n);
			string price = trimCopy(p);
			string lower = name;
			for (char& ch : lower) ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
			if (lower.find(q) == string::npos)
				continue;

			string key = cat + "|" + lower + "|" + price;
			bool already = false;
			for (int i = 0; i < seenCount; i++)
			{
				if (seenKeys[i] == key) { already = true; break; }
			}
			if (already) continue;
			if (seenCount < 2000)
				seenKeys[seenCount++] = key;

			any = true;
			contentPrint("[" + cat + "]  " + name + "  | " + price + "  | Stock: " + trimCopy(qty));
		}
	}
	if (!any)
		infoMsg("No products matched \"" + query + "\"");
	return any;
}
