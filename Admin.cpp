#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include "Admin.h"
#include "Items.h"
#include "Other_Fun.h"

using namespace std;

Admin::Admin()
{
	currentUser = "";
}

bool Admin::hasRegisteredAdmin()
{
	return countAdmins() >= 1;
}

int Admin::countAdmins()
{
	ifstream in("admin_accounts.txt");
	if (!in.is_open())
		return 0;
	string u, p;
	int n = 0;
	try
	{
		while (getline(in, u))
		{
			if (trimCopy(u).empty())
				continue;
			if (!getline(in, p))
				p = "";
			n++;
		}
	}
	catch (...)
	{
		return n;
	}
	return n;
}

void Admin::registraion()
{
	clearScreen();
	sectionTitle("ADMIN REGISTRATION", 5);
	contentPrint("Only one admin account can be created for this portal.");
	cout << "\n";
	infoMsg("Type 0 at username to go back");

	if (hasRegisteredAdmin())
	{
		errorMsg("An admin account is already registered. Please use Login.");
		return;
	}

	string user;
	if (readTokenOrCancel("Username (6-16 chars, 0 = Back):   ", user))
		return;
	contentPrint("Password:   ", false);
	string pass = getPasswordMasked();
	contentPrint("Confirm:    ", false);
	string conf = getPasswordMasked();
	if (!validCredential(user, pass, conf))
	{
		errorMsg("Invalid credentials (6-16 chars, no spaces, passwords must match)");
		return;
	}

	ofstream out("admin_accounts.txt", ios::trunc);
	if (!out.is_open())
	{
		errorMsg("Could not save admin account. Please try again.");
		return;
	}
	out << user << "\n" << pass << "\n";
	out.close();
	successMsg("Admin account created successfully. You can now login.");
}

bool Admin::login()
{
	clearScreen();
	sectionTitle("ADMIN LOGIN", 4);
	infoMsg("Type 0 at username to go back");

	if (!hasRegisteredAdmin())
	{
		errorMsg("No admin account exists yet. Please choose Registration first.");
		return false;
	}

	if (readTokenOrCancel("Username (0 = Back):   ", login_name))
		return false;
	contentPrint("Password:   ", false);
	login_pass = getPasswordMasked();

	ifstream in("admin_accounts.txt");
	if (!in.is_open())
	{
		errorMsg("Could not read admin account data. Please try again.");
		return false;
	}

	string u, p;
	try
	{
		while (getline(in, u) && getline(in, p))
		{
			if (trimCopy(u) == trimCopy(login_name) && p == login_pass)
			{
				currentUser = trimCopy(login_name);
				successMsg("ADMIN LOGIN SUCCESSFUL");
				return true;
			}
		}
	}
	catch (...)
	{
		errorMsg("Failed to read admin accounts");
		return false;
	}
	errorMsg("Invalid admin credentials");
	return false;
}

bool Admin::admin_Reg_Log_Menu()
{
	while (true)
	{
		clearScreen();
		banner("ADMIN PORTAL", 4);
		contentPrint("1)    Registration");
		cout << "\n";
		contentPrint("2)    Login");
		cout << "\n";
		contentPrint("3)    Go Back");
		cout << "\n";
		if (hasRegisteredAdmin())
			infoMsg("Admin account is already set up - please use Login");
		else
			infoMsg("No admin account yet - use Registration to create one");

		int choice = readIntInRange("Enter Your Choice:   ", 1, 3);
		if (choice == 1)
		{
			registraion();
			pauseEnter();
		}
		else if (choice == 2)
		{
			if (login())
			{
				admin_menu();
				return true;
			}
			pauseEnter();
		}
		else if (choice == 3)
		{
			return false;
		}
	}
}

void Admin::viewUsers()
{
	clearScreen();
	sectionTitle("VIEW ALL USERS", 5);

	setColor(3);
	cout << "\n";
	centerPrint("=== CUSTOMERS ===");
	cout << "\n";
	setDefaultColor();
	ifstream cust("customer_account_save.txt");
	string u, p;
	int i = 1;
	while (getline(cust, u))
	{
		if (u.empty()) continue;
		if (!getline(cust, p)) p = "";
		printNumberedLine(cout, i++, string("Username: ") + u, 2);
	}
	if (i == 1) contentPrint("(none)");

	setColor(3);
	cout << "\n";
	centerPrint("=== VENDORS ===");
	cout << "\n";
	setDefaultColor();
	ifstream vend("vendor_accounts.txt");
	string company;
	i = 1;
	while (getline(vend, u))
	{
		if (u.empty()) continue;
		getline(vend, p);
		getline(vend, company);
		printNumberedLine(cout, i++, string("Username: ") + u + "  |  Company: " + company, 2);
	}
	if (i == 1) contentPrint("(none)");

	setColor(3);
	cout << "\n";
	centerPrint("=== ADMINS ===");
	cout << "\n";
	setDefaultColor();
	ifstream adm("admin_accounts.txt");
	i = 1;
	while (getline(adm, u) && getline(adm, p))
	{
		printNumberedLine(cout, i++, string("Username: ") + u, 2);
	}
}

void Admin::manageCategories()
{
	while (true)
	{
		clearScreen();
		sectionTitle("MANAGE CATEGORIES", 5);
		Item::listCategories();
		cout << "\n";
		contentPrint("1) Add Category");
		contentPrint("2) Remove Category");
		contentPrint("3) Back");
		int ch = readIntInRange("\nChoice:   ", 1, 3);
		if (ch == 3) return;
		else if (ch == 1)
		{
			clearScreen();
			sectionTitle("ADD CATEGORY", 5);
			string name;
			if (readLineOrCancel("New category name (0 = Back):   ", name))
			{
				pauseEnter();
				continue;
			}
			Item::addCategory(name);
			pauseEnter();
		}
		else if (ch == 2)
		{
			clearScreen();
			sectionTitle("REMOVE CATEGORY", 5);
			Item::listCategories();
			cout << "\n";
			string name;
			if (readLineOrCancel("Category name to remove (0 = Back):   ", name))
			{
				pauseEnter();
				continue;
			}
			Item::removeCategory(name);
			pauseEnter();
		}
	}
}

void Admin::manageProducts()
{
	while (true)
	{
		clearScreen();
		sectionTitle("MANAGE PRODUCTS", 5);
		Item::listCategories();
		cout << "\n";
		contentPrint("1) View products in category");
		contentPrint("2) Add product");
		contentPrint("3) Update product");
		contentPrint("4) Remove product");
		contentPrint("5) Back");
		int ch = readIntInRange("\nChoice:   ", 1, 5);
		if (ch == 5) return;

		clearScreen();
		if (ch == 1) sectionTitle("VIEW PRODUCTS", 5);
		else if (ch == 2) sectionTitle("ADD PRODUCT", 5);
		else if (ch == 3) sectionTitle("UPDATE PRODUCT", 5);
		else sectionTitle("REMOVE PRODUCT", 5);
		Item::listCategories();
		cout << "\n";

		string cat;
		if (readLineOrCancel("Category name (0 = Back):   ", cat))
		{
			pauseEnter();
			continue;
		}
		if (ch == 1)
		{
			clearScreen();
			sectionTitle("PRODUCTS - " + cat, 5);
			Item::viewCategoryProducts(cat);
			pauseEnter();
		}
		else if (ch == 2)
		{
			string name;
			if (readLineOrCancel("Product name (0 = Back):   ", name))
			{
				pauseEnter();
				continue;
			}
			string price = trimCopy(readLine("Price label (e.g. 250 PKR):   "));
			int qty = readIntInRange("Quantity:   ", 0, 99999);
			Item::addProduct(cat, name, price, qty);
			pauseEnter();
		}
		else if (ch == 3)
		{
			clearScreen();
			sectionTitle("UPDATE PRODUCT - " + cat, 5);
			Item::viewCategoryProducts(cat);
			cout << "\n";
			string name;
			if (readLineOrCancel("Existing product name (0 = Back):   ", name))
			{
				pauseEnter();
				continue;
			}
			string price = trimCopy(readLine("New price label:   "));
			int qty = readIntInRange("New quantity:   ", 0, 99999);
			Item::updateProduct(cat, name, price, qty);
			pauseEnter();
		}
		else if (ch == 4)
		{
			clearScreen();
			sectionTitle("REMOVE PRODUCT - " + cat, 5);
			Item::viewCategoryProducts(cat);
			cout << "\n";
			string name;
			if (readLineOrCancel("Product name to remove (0 = Back):   ", name))
			{
				pauseEnter();
				continue;
			}
			Item::removeProduct(cat, name);
			pauseEnter();
		}
	}
}

void Admin::viewOrdersAndStats()
{
	clearScreen();
	sectionTitle("ORDERS & STATISTICS", 3);
	ifstream in("orders.txt");
	if (!in.is_open())
	{
		infoMsg("No orders recorded yet");
		return;
	}

	string line;
	int count = 0;
	double revenue = 0;
	setDefaultColor();
	cout << "\n";

	while (getline(in, line))
	{
		if (trimCopy(line).empty())
			continue;

		string id, user, totalStr, date, items;
		if (!parseOrderLine(line, id, user, totalStr, date, items))
			continue;

		count++;
		double total = 0;
		parseDoubleSafe(totalStr, total);
		revenue += total;

		printOrderBlock(id, user, totalStr, date, items);
	}

	if (count == 0)
	{
		infoMsg("No orders recorded yet");
		return;
	}

	setColor(15);
	cout << "\n";
	contentPrint("==============================================");
	contentPrint("Total Orders : " + to_string(count));
	contentPrint("Total Revenue: Rs. " + to_string(static_cast<int>(revenue)));
	contentPrint("==============================================");
	setDefaultColor();
}

bool Admin::admin_menu()
{
	while (true)
	{
		clearScreen();
		banner("ADMIN DASHBOARD - " + currentUser, 4);
		setColor(5);
		contentPrint("1)  View All Users");
		cout << "\n";
		contentPrint("2)  Manage Categories");
		cout << "\n";
		contentPrint("3)  Manage Products");
		cout << "\n";
		contentPrint("4)  View Orders & Stats");
		cout << "\n";
		contentPrint("5)  Logout");
		cout << "\n";
		setDefaultColor();
		int ch = readIntInRange("Enter choice:   ", 1, 5);

		if (ch == 1) { viewUsers(); pauseEnter(); }
		else if (ch == 2) { manageCategories(); }
		else if (ch == 3) { manageProducts(); }
		else if (ch == 4) { viewOrdersAndStats(); pauseEnter(); }
		else if (ch == 5) { successMsg("Admin logged out"); return false; }
	}
}
