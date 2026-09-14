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

bool Item::print_Items_Menu()
{
	do
	{
		Item items[500];
		int arrayofcount[100] = { 0 };
		int str_to_int, i_q, str_to_int1;
		ifstream read1;
		string filename;
		string Item_Category[100];
		int itemcount = 1, i = 1, itemcount1 = 1, itemref;
		string str;

	label:
		clearScreen();
		sectionTitle("ORDER PLACING — BROWSE CATEGORIES", 5);
		setColor(0);
		cout << "                         Select a category (or 0 to go back)\n\n";

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
			if (itemcount < 10)
				cout << "                                            0" << itemcount << ")  " << str << "\n\n";
			else
				cout << "                                            " << itemcount << ")  " << str << "\n\n";
			arrayofcount[i++] = itemcount++;
		}
		read.close();

		int choice = readIntInRange("\n                         GO BACK = 0   |   Enter category #:   ", 0, itemcount - 1);
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
		cout << left;
		cout << "                         " << setw(10) << "ITEM #"
			<< setw(28) << "ITEM NAME"
			<< setw(20) << "PRICE"
			<< setw(12) << "STOCK" << "\n\n";

		itemcount1 = 1;
		while (getline(read1, items[itemcount1].itemnames, '-') &&
			getline(read1, items[itemcount1].Items_Price, ',') &&
			getline(read1 >> ws, items[itemcount1].Items_Quantity))
		{
			str_to_int = stoi(items[itemcount1].Items_Quantity);
			cout << "                         " << setw(10) << itemcount1
				<< setw(28) << items[itemcount1].itemnames
				<< setw(20) << items[itemcount1].Items_Price
				<< setw(12) << str_to_int << "\n";
			itemcount1++;
		}
		read1.close();

		do
		{
			itemref = readIntInRange("\n                         Add ITEM # to cart (0 = back to categories):   ", 0, itemcount1 - 1);
			if (itemref == 0)
			{
				clearScreen();
				break;
			}

			str_to_int1 = stoi(items[itemref].Items_Quantity);
			if (str_to_int1 == 0)
			{
				infoMsg("Item Is Not Available");
				continue;
			}

			i_q = readIntInRange("                         Enter quantity:   ", 1, str_to_int1);
			str_to_int1 -= i_q;
			string new_val = to_string(str_to_int1);
			c.add_item(items[itemref].itemnames, items[itemref].Items_Price, i_q);
			update_items(filename, items[itemref].itemnames, items[itemref].Items_Price, new_val);
			items[itemref].Items_Quantity = new_val;
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

	while (getline(inputFile, items[itemcount1].itemnames, '-') &&
		getline(inputFile, items[itemcount1].Items_Price, ',') &&
		getline(inputFile >> ws, items[itemcount1].Items_Quantity))
	{
		if (items[itemcount1].itemnames == name && items[itemcount1].Items_Price == price)
			tempFile << items[itemcount1].itemnames << '-' << items[itemcount1].Items_Price << ", " << newValue;
		else
			tempFile << items[itemcount1].itemnames << '-' << items[itemcount1].Items_Price << ", " << items[itemcount1].Items_Quantity;

		if (!inputFile.eof())
			tempFile << "\n";
		itemcount1++;
	}

	inputFile.close();
	tempFile.close();
	remove(file_name.c_str());
	rename("temp.txt", file_name.c_str());
}

bool Item::Display_Cart()
{
	clearScreen();
	sectionTitle("YOUR CART", 5);
	c.DisplayItems();
	readIntInRange("\n                         GO BACK, PRESS 0:   ", 0, 0);
	return false;
}

bool Item::Item_Remove_from_Cart()
{
	clearScreen();
	sectionTitle("REMOVE FROM CART", 4);
	c.DisplayItems();
	if (!c.isEmpty())
		c.remove_item();
	readIntInRange("\n                         GO BACK, PRESS 0:   ", 0, 0);
	return false;
}

bool Item::Modify_Cart_Item()
{
	clearScreen();
	sectionTitle("MODIFY CART QUANTITY", 6);
	c.modify_quantity();
	readIntInRange("\n                         GO BACK, PRESS 0:   ", 0, 0);
	return false;
}

bool Item::Search_Catalog()
{
	clearScreen();
	sectionTitle("SEARCH PRODUCTS", 3);
	string query = readLine("                         Enter search keyword:   ");
	if (query.empty())
	{
		errorMsg("Empty search");
		return false;
	}
	searchAllCatalogs(query);
	cout << "\n";
	sectionTitle("SEARCH IN CART", 5);
	c.Search(query);
	readIntInRange("\n                         GO BACK, PRESS 0:   ", 0, 0);
	return false;
}

bool Item::Bill()
{
	clearScreen();
	sectionTitle("TOTAL BILL (Tax 5% + Delivery 5%)", 3);
	c.Bill(5.0, 5.0);
	readIntInRange("\n                         GO BACK, PRESS 0:   ", 0, 0);
	return false;
}

void Item::reset()
{
	c.reset_data();
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
	setColor(0);
	cout << "\n";
	while (getline(read, str))
	{
		if (str.empty()) continue;
		cout << "                         " << n++ << ")  " << str << "\n";
	}
	return true;
}

bool Item::addCategory(const string& name)
{
	if (name.empty()) return false;
	ifstream check("ItemsCategory.txt");
	string line;
	while (getline(check, line))
	{
		if (line == name)
		{
			errorMsg("Category already exists");
			return false;
		}
	}
	check.close();

	ofstream out("ItemsCategory.txt", ios::app);
	out << "\n" << name;
	out.close();

	ofstream cat(name + ".txt", ios::app);
	cat.close();
	successMsg("Category added: " + name);
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
	bool empty = true;
	{
		ifstream check(path);
		if (check.is_open())
		{
			check.seekg(0, ios::end);
			empty = (check.tellg() == 0);
		}
	}
	ofstream out(path, ios::app);
	if (!out.is_open())
	{
		errorMsg("Cannot open category file");
		return false;
	}
	if (!empty) out << "\n";
	out << name << " - " << priceLabel << ", " << qty;
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
	while (getline(in, n, '-') && getline(in, p, ',') && getline(in >> ws, q))
	{
		// trim trailing spaces from name comparison
		string trimmed = n;
		while (!trimmed.empty() && trimmed.back() == ' ') trimmed.pop_back();
		string target = name;
		while (!target.empty() && target.back() == ' ') target.pop_back();

		if (!first) temp << "\n";
		first = false;

		if (trimmed == target || n == name || n.find(name) != string::npos)
		{
			found = true;
			temp << name << " - " << newPriceLabel << ", " << newQty;
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
	while (getline(in, n, '-') && getline(in, p, ',') && getline(in >> ws, q))
	{
		string trimmed = n;
		while (!trimmed.empty() && trimmed.back() == ' ') trimmed.pop_back();
		if (trimmed == name || n.find(name) != string::npos)
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
	setColor(0);
	cout << left << "\n                         " << setw(10) << "#"
		<< setw(28) << "NAME" << setw(20) << "PRICE" << setw(10) << "QTY" << "\n\n";
	string n, p, q;
	int i = 1;
	while (getline(in, n, '-') && getline(in, p, ',') && getline(in >> ws, q))
	{
		cout << "                         " << setw(10) << i++
			<< setw(28) << n << setw(20) << p << setw(10) << q << "\n";
	}
	return true;
}

bool Item::searchAllCatalogs(const string& query)
{
	ifstream cats("ItemsCategory.txt");
	string cat;
	string q = query;
	for (char& ch : q) ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
	bool any = false;
	setColor(0);
	while (getline(cats, cat))
	{
		if (cat.empty()) continue;
		ifstream in(cat + ".txt");
		if (!in.is_open()) continue;
		string n, p, qty;
		while (getline(in, n, '-') && getline(in, p, ',') && getline(in >> ws, qty))
		{
			string lower = n;
			for (char& ch : lower) ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
			if (lower.find(q) != string::npos)
			{
				any = true;
				cout << "                         [" << cat << "]  " << n
					<< "  | " << p << "  | Stock: " << qty << "\n";
			}
		}
	}
	if (!any)
		infoMsg("No products matched \"" + query + "\"");
	return any;
}
