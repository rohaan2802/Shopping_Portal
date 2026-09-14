#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Vendor.h"
#include "Items.h"
#include "Other_Fun.h"

using namespace std;

Vendor::Vendor()
{
	company = "";
	currentUser = "";
}

void Vendor::registraion()
{
	clearScreen();
	sectionTitle("VENDOR REGISTRATION", 5);
again:
	reg_name = readToken("                         Username (6-16):   ");
	cout << "                         Password:   ";
	reg_password = getPasswordMasked();
	cout << "                         Confirm:    ";
	confirm_pass = getPasswordMasked();
	company = readLine("                         Company name:   ");

	if (!validCredential(reg_name, reg_password, confirm_pass) || company.empty())
	{
		errorMsg("Invalid data — username/password 6-16 chars, no spaces; company required");
		goto again;
	}

	ifstream in("vendor_accounts.txt");
	string u, p, c;
	while (getline(in, u))
	{
		if (trimCopy(u).empty()) continue;
		getline(in, p);
		getline(in, c);
		if (trimCopy(u) == reg_name)
		{
			errorMsg("Vendor already registered — please login");
			return;
		}
	}
	in.close();

	ofstream out("vendor_accounts.txt", ios::app);
	out << reg_name << "\n" << reg_password << "\n" << company << "\n";
	out.close();

	// Track vendor ownership file header
	ofstream own("vendor_products.txt", ios::app);
	own.close();

	successMsg("VENDOR REGISTERED SUCCESSFULLY");
}

bool Vendor::login()
{
	clearScreen();
	sectionTitle("VENDOR LOGIN", 5);
	login_name = readToken("                         Username:   ");
	cout << "                         Password:   ";
	login_pass = getPasswordMasked();

	ifstream in("vendor_accounts.txt");
	string u, p, c;
	while (getline(in, u))
	{
		if (u.empty()) continue;
		getline(in, p);
		getline(in, c);
		if (u == login_name && p == login_pass)
		{
			currentUser = u;
			company = c;
			successMsg("VENDOR LOGIN SUCCESSFUL — " + company);
			return true;
		}
	}
	errorMsg("Invalid vendor credentials");
	return false;
}

void Vendor::viewOwnProducts()
{
	clearScreen();
	sectionTitle("MY PRODUCTS — " + company, 3);
	ifstream in("vendor_products.txt");
	if (!in.is_open())
	{
		infoMsg("No products registered yet");
		return;
	}
	string line;
	int n = 0;
	setColor(0);
	while (getline(in, line))
	{
		if (line.empty()) continue;
		stringstream ss(line);
		string vendor, cat, name, price, qty;
		getline(ss, vendor, '|');
		getline(ss, cat, '|');
		getline(ss, name, '|');
		getline(ss, price, '|');
		getline(ss, qty, '|');
		if (vendor == currentUser)
		{
			n++;
			cout << "                         " << n << ") [" << cat << "] " << name
				<< "  |  " << price << "  |  Stock: " << qty << "\n";
		}
	}
	if (n == 0) infoMsg("You have no products yet");
}

void Vendor::manageOwnStock()
{
	while (true)
	{
		clearScreen();
		sectionTitle("VENDOR STOCK MANAGEMENT — " + company, 5);
		cout << "                         1) View my products\n\n";
		cout << "                         2) Add product to category\n\n";
		cout << "                         3) Update my product stock/price\n\n";
		cout << "                         4) Remove my product\n\n";
		cout << "                         5) Back\n\n";
		int ch = readIntInRange("                         Choice:   ", 1, 5);
		if (ch == 5) return;

		if (ch == 1)
		{
			viewOwnProducts();
			pauseEnter();
		}
		else if (ch == 2)
		{
			Item::listCategories();
			string cat = readLine("\n                         Category:   ");
			string name = readLine("                         Product name:   ");
			string price = readLine("                         Price label (e.g. 500 PKR):   ");
			int qty = readIntInRange("                         Stock quantity:   ", 0, 99999);

			// Reject duplicate ownership rows for this vendor
			ifstream ownCheck("vendor_products.txt");
			string line;
			bool alreadyOwns = false;
			while (getline(ownCheck, line))
			{
				if (trimCopy(line).empty()) continue;
				stringstream ss(line);
				string vendor, vcat, vname, vprice, vqty;
				getline(ss, vendor, '|');
				getline(ss, vcat, '|');
				getline(ss, vname, '|');
				getline(ss, vprice, '|');
				getline(ss, vqty, '|');
				if (vendor == currentUser && trimCopy(vcat) == trimCopy(cat) && trimCopy(vname) == trimCopy(name))
				{
					alreadyOwns = true;
					break;
				}
			}
			ownCheck.close();
			if (alreadyOwns)
			{
				errorMsg("You already listed that product — use Update instead");
				pauseEnter();
				continue;
			}

			if (Item::addProduct(cat, name, price, qty))
			{
				ofstream own("vendor_products.txt", ios::app);
				own << currentUser << "|" << trimCopy(cat) << "|" << trimCopy(name)
					<< "|" << trimCopy(price) << "|" << qty << "\n";
				own.close();
			}
			pauseEnter();
		}
		else if (ch == 3)
		{
			viewOwnProducts();
			string cat = readLine("\n                         Category:   ");
			string name = readLine("                         Product name:   ");
			string price = readLine("                         New price label:   ");
			int qty = readIntInRange("                         New stock:   ", 0, 99999);

			// Verify ownership
			ifstream in("vendor_products.txt");
			ofstream temp("temp_vendor.txt");
			string line;
			bool owned = false;
			while (getline(in, line))
			{
				stringstream ss(line);
				string vendor, vcat, vname, vprice, vqty;
				getline(ss, vendor, '|');
				getline(ss, vcat, '|');
				getline(ss, vname, '|');
				getline(ss, vprice, '|');
				getline(ss, vqty, '|');
				if (vendor == currentUser && vcat == cat && vname == name)
				{
					owned = true;
					temp << vendor << "|" << cat << "|" << name << "|" << price << "|" << qty << "\n";
				}
				else
				{
					temp << line << "\n";
				}
			}
			in.close();
			temp.close();
			if (!owned)
			{
				remove("temp_vendor.txt");
				errorMsg("You do not own that product");
			}
			else
			{
				remove("vendor_products.txt");
				rename("temp_vendor.txt", "vendor_products.txt");
				Item::updateProduct(cat, name, price, qty);
			}
			pauseEnter();
		}
		else if (ch == 4)
		{
			viewOwnProducts();
			string cat = readLine("\n                         Category:   ");
			string name = readLine("                         Product name to remove:   ");

			ifstream in("vendor_products.txt");
			ofstream temp("temp_vendor.txt");
			string line;
			bool owned = false;
			while (getline(in, line))
			{
				stringstream ss(line);
				string vendor, vcat, vname, vprice, vqty;
				getline(ss, vendor, '|');
				getline(ss, vcat, '|');
				getline(ss, vname, '|');
				getline(ss, vprice, '|');
				getline(ss, vqty, '|');
				if (vendor == currentUser && vcat == cat && vname == name)
				{
					owned = true;
					continue;
				}
				temp << line << "\n";
			}
			in.close();
			temp.close();
			if (!owned)
			{
				remove("temp_vendor.txt");
				errorMsg("You do not own that product");
			}
			else
			{
				remove("vendor_products.txt");
				rename("temp_vendor.txt", "vendor_products.txt");
				Item::removeProduct(cat, name);
			}
			pauseEnter();
		}
	}
}

bool Vendor::vendor_menu()
{
	while (true)
	{
		clearScreen();
		banner("VENDOR DASHBOARD — " + currentUser + " (" + company + ")", 5);
		setColor(0);
		cout << "                         1)  Manage My Products / Stock\n\n";
		cout << "                         2)  View Catalog Categories\n\n";
		cout << "                         3)  Search Catalog\n\n";
		cout << "                         4)  Logout\n\n";
		int ch = readIntInRange("                         Enter choice:   ", 1, 4);
		if (ch == 1) manageOwnStock();
		else if (ch == 2)
		{
			clearScreen();
			sectionTitle("CATEGORIES", 3);
			Item::listCategories();
			pauseEnter();
		}
		else if (ch == 3)
		{
			clearScreen();
			string q = readLine("                         Search:   ");
			Item::searchAllCatalogs(q);
			pauseEnter();
		}
		else if (ch == 4)
		{
			successMsg("Vendor logged out");
			return false;
		}
	}
}
