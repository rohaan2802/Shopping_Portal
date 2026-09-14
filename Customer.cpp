#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <windows.h>
#include <conio.h>
#include "Cart.h"
#include "Items.h"
#include "Customer.h"
#include "Other_Fun.h"

using namespace std;

extern Cart c;

Customer::Customer()
{
	currentUser = "";
	wishlist_size = 0;
	for (int i = 0; i < 100; i++) wishlist[i] = "";
}

bool Customer::customer_Reg_Log_Menu()
{
label1:
	clearScreen();
	banner("CUSTOMER GATEWAY", 3);
	setColor(0);
	cout << "                                                    1)    Registration\n\n";
	cout << "                                                    2)    Login\n\n";
	cout << "                                                    3)    Forgot Password\n\n";
	cout << "                                                    4)    Go Back\n\n";
	int choice = readIntInRange("                                      Enter Your Choice here                   ", 1, 4);

	if (choice == 1)
	{
		registraion();
		pauseEnter();
		goto label1;
	}
	if (choice == 2)
	{
		if (login())
		{
			show_customer_menu();
			c.restock_all_and_clear();
			goto label1;
		}
		else
		{
			pauseEnter();
			goto label1;
		}
	}
	if (choice == 3)
	{
		forgotPassword();
		pauseEnter();
		goto label1;
	}
	if (choice == 4)
		return false;

	return false;
}

void Customer::registraion()
{
	ofstream write_customer_account_reg;
	ifstream read_customer_account_reg;
	clearScreen();
	sectionTitle("CUSTOMER REGISTRATION", 5);
again32:
	reg_name = readToken("                                                Enter Your User_Name                       ");
	cout << "                                                Enter Your password                        ";
	reg_password = getPasswordMasked();
	cout << "                                                Enter Confirm_Password                     ";
	confirm_pass = getPasswordMasked();

	if (!validCredential(reg_name, reg_password, confirm_pass))
	{
		errorMsg("USERNAME/PASSWORD 6-16 chars, no spaces, confirm must match");
		goto again32;
	}

	read_customer_account_reg.open("customer_account_save.txt");
	string existingUser, existingPass;
	if (read_customer_account_reg.is_open())
	{
		while (getline(read_customer_account_reg, existingUser))
		{
			if (trimCopy(existingUser).empty()) continue;
			if (!getline(read_customer_account_reg, existingPass)) existingPass = "";
			if (trimCopy(existingUser) == reg_name)
			{
				errorMsg("Account Already Registered — please login");
				read_customer_account_reg.close();
				return;
			}
		}
	}
	read_customer_account_reg.close();

	write_customer_account_reg.open("customer_account_save.txt", ios::app);
	write_customer_account_reg << reg_name << "\n" << reg_password << "\n";
	write_customer_account_reg.close();
	successMsg("REGISTERED SUCCESSFULLY");
}

bool Customer::login()
{
	ifstream read_customer_account_log("customer_account_save.txt");
	clearScreen();
	sectionTitle("CUSTOMER LOGIN", 5);
	if (!read_customer_account_log.is_open())
	{
		errorMsg("NO REGISTRATION/ACCOUNT FOUND");
		return false;
	}

	login_name = readToken("                                                      Enter Your User_Name                   ");
	cout << "                                                      Enter Your password                    ";
	login_pass = getPasswordFromUser();

	string s1, s2;
	while (getline(read_customer_account_log, s1))
	{
		if (s1.empty()) continue;
		if (!getline(read_customer_account_log, s2)) s2 = "";
		if (s1 == login_name && s2 == login_pass)
		{
			currentUser = login_name;
			successMsg("LOGIN SUCCESSFUL — Welcome " + currentUser);
			return true;
		}
	}
	errorMsg("NO REGISTRATION/ACCOUNT FOUND");
	return false;
}

string Customer::getPasswordFromUser()
{
	return getPasswordMasked();
}

void Customer::forgotPassword()
{
	clearScreen();
	sectionTitle("FORGOT PASSWORD", 4);
again32:
	string searchString = readToken("                                                Enter Your User_Name                         ");
	cout << "                                                Enter Your New_Password                      ";
	string newValue = getPasswordMasked();
	cout << "                                                Enter Confirm New_Password                   ";
	string confirm_new_val = getPasswordMasked();

	if (!validCredential(searchString, newValue, confirm_new_val))
	{
		errorMsg("Invalid password rules — try again");
		goto again32;
	}
	updateValueInFile("customer_account_save.txt", searchString, newValue);
}

void Customer::updateValueInFile(const string& filename, const string& searchString, const string& newValue)
{
	ifstream inputFile(filename);
	if (!inputFile)
	{
		errorMsg("Unable to open the file");
		return;
	}

	ofstream tempFile("temp.txt");
	string line;
	bool found = false;

	while (getline(inputFile, line))
	{
		if (line == searchString)
		{
			found = true;
			tempFile << line << "\n";
			getline(inputFile, line); // skip old password
			tempFile << newValue << "\n";
		}
		else
		{
			tempFile << line << "\n";
		}
	}

	inputFile.close();
	tempFile.close();

	if (!found)
	{
		remove("temp.txt");
		errorMsg("User_Name not found");
		return;
	}

	remove(filename.c_str());
	if (rename("temp.txt", filename.c_str()) == 0)
		successMsg("PASSWORD UPDATED SUCCESSFULLY");
	else
		errorMsg("Error renaming the file");
}

void Customer::checkoutAndSaveOrder()
{
	clearScreen();
	sectionTitle("CHECKOUT & BILL", 3);
	if (!c.Bill(5.0, 5.0))
	{
		pauseEnter();
		return;
	}

	cout << "\n";
	int confirm = readIntInRange("                         Place order? 1 = YES, 0 = NO:   ", 0, 1);
	if (confirm == 0)
	{
		infoMsg("Checkout cancelled");
		pauseEnter();
		return;
	}

	// Generate order id
	int nextId = 1001;
	ifstream check("orders.txt");
	string line;
	while (getline(check, line))
	{
		if (line.empty()) continue;
		nextId++;
	}
	check.close();

	ostringstream details;
	for (int i = 0; i < c.size(); i++)
	{
		if (i) details << "; ";
		details << c.getName(i) << " x" << c.getQty(i);
	}

	time_t now = time(nullptr);
	tm local{};
	localtime_s(&local, &now);
	char buf[64];
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &local);

	ofstream out("orders.txt", ios::app);
	out << nextId << "|" << currentUser << "|" << (int)c.lastBillTotal()
		<< "|" << details.str() << " @ " << buf << "\n";
	out.close();

	successMsg("ORDER PLACED — ID #" + to_string(nextId));
	c.reset_data();
	pauseEnter();
}

void Customer::viewOrderHistory()
{
	clearScreen();
	sectionTitle("ORDER HISTORY — " + currentUser, 5);
	ifstream in("orders.txt");
	if (!in.is_open())
	{
		infoMsg("No orders found");
		return;
	}
	string line;
	int n = 0;
	setColor(0);
	while (getline(in, line))
	{
		if (line.empty()) continue;
		stringstream ss(line);
		string id, user, total, rest;
		getline(ss, id, '|');
		getline(ss, user, '|');
		getline(ss, total, '|');
		getline(ss, rest);
		if (user == currentUser)
		{
			n++;
			cout << "                         #" << id << "  |  Total: Rs. " << total << "  |  " << rest << "\n\n";
		}
	}
	if (n == 0) infoMsg("You have no past orders");
}

void Customer::manageWishlist()
{
	clearScreen();
	sectionTitle("WISHLIST", 6);
	if (wishlist_size == 0)
		infoMsg("Wishlist is empty");
	else
	{
		for (int i = 0; i < wishlist_size; i++)
			cout << "                         " << (i + 1) << ")  " << wishlist[i] << "\n";
	}
	cout << "\n                         1) Add item name to wishlist\n";
	cout << "                         2) Remove from wishlist\n";
	cout << "                         3) Back\n";
	int ch = readIntInRange("                         Choice:   ", 1, 3);
	if (ch == 1)
	{
		string name = trimCopy(readLine("                         Item name:   "));
		if (name.empty())
		{
			errorMsg("Item name required");
		}
		else
		{
			bool exists = false;
			for (int i = 0; i < wishlist_size; i++)
			{
				if (trimCopy(wishlist[i]) == name)
				{
					exists = true;
					break;
				}
			}
			if (exists)
				errorMsg("Already on wishlist");
			else if (wishlist_size < 100)
			{
				wishlist[wishlist_size++] = name;
				successMsg("Added to wishlist");
			}
			else
				errorMsg("Wishlist is full");
		}
	}
	else if (ch == 2 && wishlist_size > 0)
	{
		int idx = readIntInRange("                         Item # to remove:   ", 1, wishlist_size);
		idx--;
		for (int i = idx; i < wishlist_size - 1; i++)
			wishlist[i] = wishlist[i + 1];
		wishlist_size--;
		successMsg("Removed from wishlist");
	}
}

bool Customer::show_customer_menu()
{
	Item items;
	while (true)
	{
		clearScreen();
		banner("CUSTOMER VIEW — " + currentUser, 5);
		setColor(0);
		cout << "                                                  1)  Place An Order / Browse\n\n";
		cout << "                                                  2)  Modify Cart Quantity\n\n";
		cout << "                                                  3)  Display Cart Items\n\n";
		cout << "                                                  4)  Search Products\n\n";
		cout << "                                                  5)  Remove An Item From Cart\n\n";
		cout << "                                                  6)  Show Bill / Checkout\n\n";
		cout << "                                                  7)  Wishlist\n\n";
		cout << "                                                  8)  Order History\n\n";
		cout << "                                                  9)  Logout\n\n";

		int choice = readIntInRange("                                           Enter Your Choice here                   ", 1, 9);

		if (choice == 1)
		{
			items.print_Items_Menu();
		}
		else if (choice == 2)
		{
			items.Modify_Cart_Item();
		}
		else if (choice == 3)
		{
			items.Display_Cart();
		}
		else if (choice == 4)
		{
			items.Search_Catalog();
		}
		else if (choice == 5)
		{
			items.Item_Remove_from_Cart();
		}
		else if (choice == 6)
		{
			checkoutAndSaveOrder();
		}
		else if (choice == 7)
		{
			manageWishlist();
			pauseEnter();
		}
		else if (choice == 8)
		{
			viewOrderHistory();
			pauseEnter();
		}
		else if (choice == 9)
		{
			items.reset();
			successMsg("Logged out — cart cleared");
			return false;
		}
	}
}
