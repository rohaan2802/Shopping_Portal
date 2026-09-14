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
	string user = readToken("Username (6-16 chars):   ");
	contentPrint("Password:   ", false);
	string pass = getPasswordMasked();
	contentPrint("Confirm:    ", false);
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
	login_name = readToken("Username:   ");
	contentPrint("Password:   ", false);
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
			string name = trimCopy(readLine("New category name:   "));
			Item::addCategory(name);
			pauseEnter();
		}
		else if (ch == 2)
		{
			string name = trimCopy(readLine("Category name to remove:   "));
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
		cout << "\n";
		contentPrint("1) View products in category");
		contentPrint("2) Add product");
		contentPrint("3) Update product");
		contentPrint("4) Remove product");
		contentPrint("5) Back");
		int ch = readIntInRange("\nChoice:   ", 1, 5);
		if (ch == 5) return;

		string cat = trimCopy(readLine("Category name:   "));
		if (cat.empty())
		{
			errorMsg("Category name required");
			pauseEnter();
			continue;
		}
		if (ch == 1)
		{
			Item::viewCategoryProducts(cat);
			pauseEnter();
		}
		else if (ch == 2)
		{
			string name = trimCopy(readLine("Product name:   "));
			string price = trimCopy(readLine("Price label (e.g. 250 PKR):   "));
			int qty = readIntInRange("Quantity:   ", 0, 99999);
			Item::addProduct(cat, name, price, qty);
			pauseEnter();
		}
		else if (ch == 3)
		{
			string name = trimCopy(readLine("Existing product name:   "));
			string price = trimCopy(readLine("New price label:   "));
			int qty = readIntInRange("New quantity:   ", 0, 99999);
			Item::updateProduct(cat, name, price, qty);
			pauseEnter();
		}
		else if (ch == 4)
		{
			string name = trimCopy(readLine("Product name to remove:   "));
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
	setDefaultColor();
	cout << "\n";
	{
		ostringstream hdr;
		hdr << fitField("ID", 6, false) << "  "
			<< fitField("CUSTOMER", 14, true)
			<< fitField("TOTAL", 12, true)
			<< "DETAILS";
		contentPrint(hdr.str());
		cout << "\n";
	}

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
		double total = 0;
		parseDoubleSafe(totalStr, total);
		revenue += total;
		int idNum = 0;
		ostringstream row;
		if (parseIntSafe(id, idNum))
			row << paddedIndex(idNum, 6);
		else
			row << fitField(id, 6, false);
		row << "  " << fitField(user, 14, true)
			<< fitField("Rs. " + totalStr, 12, true)
			<< truncateFit(rest, CONTENT_WIDTH - 34);
		contentPrint(row.str());
	}

	setColor(15);
	cout << "\n\n";
	contentPrint("Total Orders : " + to_string(count));
	contentPrint("Total Revenue: Rs. " + to_string(static_cast<int>(revenue)));
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
		contentPrint("3)  Manage Products (CRUD)");
		cout << "\n";
		contentPrint("4)  View Orders & Stats");
		cout << "\n";
		contentPrint("5)  Create Another Admin");
		cout << "\n";
		contentPrint("6)  Logout");
		cout << "\n";
		setDefaultColor();
		int ch = readIntInRange("Enter choice:   ", 1, 6);

		if (ch == 1) { viewUsers(); pauseEnter(); }
		else if (ch == 2) { manageCategories(); }
		else if (ch == 3) { manageProducts(); }
		else if (ch == 4) { viewOrdersAndStats(); pauseEnter(); }
		else if (ch == 5) { registraion(); pauseEnter(); }
		else if (ch == 6) { successMsg("Admin logged out"); return false; }
	}
}
