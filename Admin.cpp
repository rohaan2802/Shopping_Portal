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
	ensureDefaultAdmin();
}

void Admin::ensureDefaultAdmin()
{
	ifstream check("admin_accounts.txt");
	if (check.good() && check.peek() != EOF)
	{
		check.close();
		return;
	}
	check.close();
	ofstream out("admin_accounts.txt");
	out << "admin\nadmin123\n";
	out.close();
}

void Admin::registraion()
{
	ensureDefaultAdmin();
	clearScreen();
	sectionTitle("CREATE ADDITIONAL ADMIN", 5);
	string user = readToken("                         Username (6-16 chars):   ");
	cout << "                         Password:   ";
	string pass = getPasswordMasked();
	cout << "                         Confirm:    ";
	string conf = getPasswordMasked();
	if (!validCredential(user, pass, conf))
	{
		errorMsg("Invalid credentials (6-16 chars, no spaces, passwords must match)");
		return;
	}
	ifstream in("admin_accounts.txt");
	string u, p;
	while (getline(in, u) && getline(in, p))
	{
		if (u == user)
		{
			errorMsg("Admin already exists");
			return;
		}
	}
	in.close();
	ofstream out("admin_accounts.txt", ios::app);
	out << user << "\n" << pass << "\n";
	out.close();
	successMsg("Admin account created");
}

bool Admin::login()
{
	ensureDefaultAdmin();
	clearScreen();
	sectionTitle("ADMIN LOGIN", 4);
	login_name = readToken("                         Username:   ");
	cout << "                         Password:   ";
	login_pass = getPasswordMasked();

	ifstream in("admin_accounts.txt");
	string u, p;
	while (getline(in, u) && getline(in, p))
	{
		if (u == login_name && p == login_pass)
		{
			currentUser = login_name;
			successMsg("ADMIN LOGIN SUCCESSFUL");
			return true;
		}
	}
	errorMsg("Invalid admin credentials");
	return false;
}

void Admin::viewUsers()
{
	clearScreen();
	sectionTitle("VIEW ALL USERS", 5);

	setColor(3);
	cout << "\n                         === CUSTOMERS ===\n\n";
	setColor(0);
	ifstream cust("customer_account_save.txt");
	string u, p;
	int i = 1;
	while (getline(cust, u))
	{
		if (u.empty()) continue;
		if (!getline(cust, p)) p = "";
		cout << "                         " << i++ << ")  Username: " << u << "\n";
	}
	if (i == 1) cout << "                         (none)\n";

	setColor(3);
	cout << "\n                         === VENDORS ===\n\n";
	setColor(0);
	ifstream vend("vendor_accounts.txt");
	string company;
	i = 1;
	while (getline(vend, u))
	{
		if (u.empty()) continue;
		getline(vend, p);
		getline(vend, company);
		cout << "                         " << i++ << ")  Username: " << u << "  |  Company: " << company << "\n";
	}
	if (i == 1) cout << "                         (none)\n";

	setColor(3);
	cout << "\n                         === ADMINS ===\n\n";
	setColor(0);
	ifstream adm("admin_accounts.txt");
	i = 1;
	while (getline(adm, u) && getline(adm, p))
	{
		cout << "                         " << i++ << ")  Username: " << u << "\n";
	}
}

void Admin::manageCategories()
{
	while (true)
	{
		clearScreen();
		sectionTitle("MANAGE CATEGORIES", 5);
		Item::listCategories();
		cout << "\n                         1) Add Category\n";
		cout << "                         2) Remove Category\n";
		cout << "                         3) Back\n";
		int ch = readIntInRange("\n                         Choice:   ", 1, 3);
		if (ch == 3) return;
		if (ch == 1)
		{
			string name = readLine("                         New category name:   ");
			Item::addCategory(name);
			pauseEnter();
		}
		else if (ch == 2)
		{
			string name = readLine("                         Category name to remove:   ");
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
		sectionTitle("MANAGE PRODUCTS (CRUD)", 5);
		Item::listCategories();
		cout << "\n                         1) View products in category\n";
		cout << "                         2) Add product\n";
		cout << "                         3) Update product\n";
		cout << "                         4) Remove product\n";
		cout << "                         5) Back\n";
		int ch = readIntInRange("\n                         Choice:   ", 1, 5);
		if (ch == 5) return;

		string cat = readLine("                         Category name:   ");
		if (ch == 1)
		{
			Item::viewCategoryProducts(cat);
			pauseEnter();
		}
		else if (ch == 2)
		{
			string name = readLine("                         Product name:   ");
			string price = readLine("                         Price label (e.g. 250 PKR):   ");
			int qty = readIntInRange("                         Quantity:   ", 0, 99999);
			Item::addProduct(cat, name, price, qty);
			pauseEnter();
		}
		else if (ch == 3)
		{
			string name = readLine("                         Existing product name:   ");
			string price = readLine("                         New price label:   ");
			int qty = readIntInRange("                         New quantity:   ", 0, 99999);
			Item::updateProduct(cat, name, price, qty);
			pauseEnter();
		}
		else if (ch == 4)
		{
			string name = readLine("                         Product name to remove:   ");
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
		infoMsg("No orders yet (orders.txt missing or empty)");
		return;
	}

	string line;
	int count = 0;
	double revenue = 0;
	setColor(0);
	cout << left << "\n                         " << setw(8) << "ID"
		<< setw(16) << "CUSTOMER"
		<< setw(14) << "TOTAL"
		<< "DETAILS\n\n";

	while (getline(in, line))
	{
		if (line.empty()) continue;
		count++;
		stringstream ss(line);
		string id, user, totalStr, rest;
		getline(ss, id, '|');
		getline(ss, user, '|');
		getline(ss, totalStr, '|');
		getline(ss, rest);
		revenue += stod(totalStr);
		cout << "                         " << setw(8) << id
			<< setw(16) << user
			<< "Rs. " << setw(10) << totalStr
			<< rest << "\n";
	}

	setColor(15);
	cout << "\n\n                         Total Orders : " << count;
	cout << "\n                         Total Revenue: Rs. " << (int)revenue << "\n";
	setColor(0);
}

bool Admin::admin_menu()
{
	while (true)
	{
		clearScreen();
		banner("ADMIN DASHBOARD — " + currentUser, 4);
		setColor(5);
		cout << "                         1)  View All Users\n\n";
		cout << "                         2)  Manage Categories\n\n";
		cout << "                         3)  Manage Products (CRUD)\n\n";
		cout << "                         4)  View Orders & Stats\n\n";
		cout << "                         5)  Create Another Admin\n\n";
		cout << "                         6)  Logout\n\n";
		int ch = readIntInRange("                         Enter choice:   ", 1, 6);

		if (ch == 1) { viewUsers(); pauseEnter(); }
		else if (ch == 2) { manageCategories(); }
		else if (ch == 3) { manageProducts(); }
		else if (ch == 4) { viewOrdersAndStats(); pauseEnter(); }
		else if (ch == 5) { registraion(); pauseEnter(); }
		else if (ch == 6) { successMsg("Admin logged out"); return false; }
	}
}
