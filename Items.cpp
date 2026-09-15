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
			if (trimCopy(str).empty()) continue;
			Item_Category[itemcount] = trimCopy(str);
			printNumberedLine(cout, itemcount, Item_Category[itemcount], 2);
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
			clearScreen();
			sectionTitle(filename, 3);
			contentPrint(productHeader());
			cout << "\n";
			for (int r = 1; r < itemcount1; r++)
			{
				int qtyShow = 0;
				parseIntSafe(items[r].Items_Quantity, qtyShow);
				contentPrint(productRow(r, items[r].itemnames, items[r].Items_Price, to_string(qtyShow)));
			}
			// Catalog files unchanged until checkout
		} while (true);

	} while (true);
}

void Item::update_items(const string& filename, const string& name, string price, const string& newValue)
{
	try
	{
		string file_name = trimCopy(filename) + ".txt";
		ifstream inputFile(file_name);
		if (!inputFile)
		{
			errorMsg("Unable to open " + file_name);
			return;
		}

		ofstream tempFile("temp.txt");
		if (!tempFile.is_open())
		{
			errorMsg("Unable to write temp file");
			return;
		}
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
			if (itemcount1 >= 1000) break;
		}

		inputFile.close();
		tempFile.close();
		remove(file_name.c_str());
		rename("temp.txt", file_name.c_str());
	}
	catch (...)
	{
		errorMsg("Failed to update catalog stock");
	}
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
	infoMsg("Type 0 to go back");
	string query;
	if (readLineOrCancel("Search keyword (0 = Back):   ", query))
	{
		return false;
	}
	clearScreen();
	sectionTitle("SEARCH RESULTS - " + query, 3);
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

bool Item::categoryExists(const string& name)
{
	string target = trimCopy(name);
	if (target.empty()) return false;
	try
	{
		ifstream read("ItemsCategory.txt");
		if (!read.is_open())
			return false;
		string line;
		while (getline(read, line))
		{
			if (trimCopy(line) == target)
				return true;
		}
	}
	catch (...)
	{
		return false;
	}
	return false;
}

bool Item::ensureCategoryFile(const string& name)
{
	string target = trimCopy(name);
	if (target.empty()) return false;
	string path = target + ".txt";
	try
	{
		ifstream check(path);
		if (check.is_open())
		{
			check.close();
			return true;
		}
		ofstream create(path, ios::app);
		if (!create.is_open())
			return false;
		create.close();
		return true;
	}
	catch (...)
	{
		return false;
	}
}

int Item::listCategoriesFromFile(string* out, int maxCount)
{
	if (!out || maxCount <= 0) return 0;
	int count = 0;
	try
	{
		ifstream read("ItemsCategory.txt");
		if (!read.is_open())
			return 0;
		string line;
		while (getline(read, line) && count < maxCount)
		{
			string cat = trimCopy(line);
			if (cat.empty()) continue;
			out[count++] = cat;
		}
	}
	catch (...)
	{
		return count;
	}
	return count;
}

bool Item::categoryHasProducts(const string& name)
{
	string target = trimCopy(name);
	if (target.empty()) return false;
	try
	{
		ifstream in(target + ".txt");
		if (!in.is_open())
			return false;
		string n, p, q;
		if (getline(in, n, '-') && getline(in, p, ',') && getline(in >> ws, q))
		{
			if (!trimCopy(n).empty())
				return true;
		}
	}
	catch (...)
	{
		return false;
	}
	return false;
}

bool Item::listCategories()
{
	try
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
			if (trimCopy(str).empty()) continue;
			printNumberedLine(cout, n++, trimCopy(str), 2);
		}
		if (n == 1)
			infoMsg("No categories registered");
		return true;
	}
	catch (...)
	{
		errorMsg("Failed to read ItemsCategory.txt");
		return false;
	}
}

bool Item::addCategory(const string& name)
{
	string target = trimCopy(name);
	if (target.empty())
	{
		errorMsg("Category name required");
		return false;
	}
	if (categoryExists(target))
	{
		errorMsg("Category already exists");
		return false;
	}

	try
	{
		bool emptyFile = true;
		{
			ifstream check("ItemsCategory.txt");
			if (check.is_open())
			{
				string line;
				while (getline(check, line))
				{
					if (!trimCopy(line).empty())
					{
						emptyFile = false;
						break;
					}
				}
			}
		}

		ofstream out("ItemsCategory.txt", ios::app);
		if (!out.is_open())
		{
			errorMsg("Cannot write ItemsCategory.txt");
			return false;
		}
		if (!emptyFile) out << "\n";
		out << target;
		out.close();

		if (!ensureCategoryFile(target))
		{
			errorMsg("Category listed but could not create file: " + target + ".txt");
			return false;
		}
		successMsg("Category added: " + target);
		return true;
	}
	catch (...)
	{
		errorMsg("Failed to add category");
		return false;
	}
}

bool Item::removeCategory(const string& name)
{
	string target = trimCopy(name);
	if (target.empty())
	{
		errorMsg("Category name required");
		return false;
	}
	if (!categoryExists(target))
	{
		errorMsg("Category not found in ItemsCategory.txt");
		return false;
	}
	// Safer UX: block delete when products still exist in the category file
	if (categoryHasProducts(target))
	{
		errorMsg("Cannot delete \"" + target + "\": products still exist");
		return false;
	}

	try
	{
		ifstream in("ItemsCategory.txt");
		if (!in.is_open())
		{
			errorMsg("ItemsCategory.txt not found");
			return false;
		}
		ofstream temp("temp.txt");
		if (!temp.is_open())
		{
			errorMsg("Cannot write temp file");
			return false;
		}
		string line;
		bool found = false;
		bool first = true;
		while (getline(in, line))
		{
			if (trimCopy(line).empty()) continue;
			if (trimCopy(line) == target)
			{
				found = true;
				continue;
			}
			if (!first) temp << "\n";
			temp << trimCopy(line);
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
		remove((target + ".txt").c_str());
		successMsg("Category removed: " + target);
		return true;
	}
	catch (...)
	{
		errorMsg("Failed to remove category");
		return false;
	}
}

bool Item::addProduct(const string& category, const string& name, const string& priceLabel, int qty)
{
	string cat = trimCopy(category);
	string target = trimCopy(name);
	string price = trimCopy(priceLabel);

	if (cat.empty())
	{
		errorMsg("Category name required");
		return false;
	}
	if (!categoryExists(cat))
	{
		errorMsg("Invalid category \"" + cat + "\": not listed in ItemsCategory.txt");
		return false;
	}
	if (!fileExists(cat + ".txt"))
	{
		errorMsg("Category file missing: " + cat + ".txt");
		return false;
	}
	if (target.empty())
	{
		errorMsg("Product name required");
		return false;
	}
	if (price.empty())
	{
		errorMsg("Price label required");
		return false;
	}
	if (qty < 0)
	{
		errorMsg("Quantity cannot be negative");
		return false;
	}

	try
	{
		string path = cat + ".txt";
		bool empty = true;
		{
			ifstream check(path);
			if (!check.is_open())
			{
				errorMsg("Cannot open category file: " + path);
				return false;
			}
			check.seekg(0, ios::end);
			empty = (check.tellg() <= 0);
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
		ofstream out(path, ios::app);
		if (!out.is_open())
		{
			errorMsg("Cannot open category file");
			return false;
		}
		if (!empty) out << "\n";
		out << target << " - " << price << ", " << qty;
		out.close();
		successMsg("Product added");
		return true;
	}
	catch (...)
	{
		errorMsg("Failed to add product");
		return false;
	}
}

bool Item::updateProduct(const string& category, const string& name, const string& newPriceLabel, int newQty)
{
	string cat = trimCopy(category);
	string target = trimCopy(name);
	string price = trimCopy(newPriceLabel);

	if (cat.empty())
	{
		errorMsg("Category name required");
		return false;
	}
	if (!categoryExists(cat))
	{
		errorMsg("Invalid category \"" + cat + "\": not listed in ItemsCategory.txt");
		return false;
	}
	if (!fileExists(cat + ".txt"))
	{
		errorMsg("Category file missing: " + cat + ".txt");
		return false;
	}
	if (target.empty())
	{
		errorMsg("Product name required");
		return false;
	}
	if (price.empty())
	{
		errorMsg("Price label required");
		return false;
	}
	if (newQty < 0)
	{
		errorMsg("Quantity cannot be negative");
		return false;
	}

	try
	{
		string path = cat + ".txt";
		ifstream in(path);
		if (!in.is_open())
		{
			errorMsg("Category file missing");
			return false;
		}
		ofstream temp("temp.txt");
		if (!temp.is_open())
		{
			errorMsg("Cannot write temp file");
			return false;
		}
		string n, p, q;
		bool found = false;
		bool first = true;
		while (getline(in, n, '-') && getline(in, p, ',') && getline(in >> ws, q))
		{
			if (!first) temp << "\n";
			first = false;

			if (trimCopy(n) == target)
			{
				found = true;
				temp << target << " - " << price << ", " << newQty;
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
	catch (...)
	{
		errorMsg("Failed to update product");
		return false;
	}
}

bool Item::removeProduct(const string& category, const string& name)
{
	string cat = trimCopy(category);
	string target = trimCopy(name);

	if (cat.empty())
	{
		errorMsg("Category name required");
		return false;
	}
	if (!categoryExists(cat))
	{
		errorMsg("Invalid category \"" + cat + "\": not listed in ItemsCategory.txt");
		return false;
	}
	if (!fileExists(cat + ".txt"))
	{
		errorMsg("Category file missing: " + cat + ".txt");
		return false;
	}
	if (target.empty())
	{
		errorMsg("Product name required");
		return false;
	}

	try
	{
		string path = cat + ".txt";
		ifstream in(path);
		if (!in.is_open())
		{
			errorMsg("Category file missing");
			return false;
		}
		ofstream temp("temp.txt");
		if (!temp.is_open())
		{
			errorMsg("Cannot write temp file");
			return false;
		}
		string n, p, q;
		bool found = false;
		bool first = true;
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
	catch (...)
	{
		errorMsg("Failed to remove product");
		return false;
	}
}

bool Item::viewCategoryProducts(const string& category)
{
	string cat = trimCopy(category);
	if (cat.empty())
	{
		errorMsg("Category name required");
		return false;
	}
	if (!categoryExists(cat))
	{
		errorMsg("Invalid category \"" + cat + "\": not listed in ItemsCategory.txt");
		return false;
	}
	try
	{
		ifstream in(cat + ".txt");
		if (!in.is_open())
		{
			errorMsg("Cannot open " + cat + ".txt (file missing)");
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
		if (i == 1)
			infoMsg("No products in this category");
		return true;
	}
	catch (...)
	{
		errorMsg("Failed to read category products");
		return false;
	}
}

bool Item::searchAllCatalogs(const string& query)
{
	string q = trimCopy(query);
	if (q.empty())
	{
		errorMsg("Empty search");
		return false;
	}
	for (char& ch : q) ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
	bool any = false;
	setDefaultColor();
	string seenKeys[2000];
	int seenCount = 0;

	try
	{
		ifstream cats("ItemsCategory.txt");
		if (!cats.is_open())
		{
			errorMsg("ItemsCategory.txt not found");
			return false;
		}
		string cat;
		while (getline(cats, cat))
		{
			cat = trimCopy(cat);
			if (cat.empty()) continue;
			ifstream in(cat + ".txt");
			if (!in.is_open())
			{
				infoMsg("Skipping missing category file: " + cat + ".txt");
				continue;
			}
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
	}
	catch (...)
	{
		errorMsg("Search failed due to a file error");
		return false;
	}

	if (!any)
		infoMsg("No products matched \"" + query + "\"");
	return any;
}
