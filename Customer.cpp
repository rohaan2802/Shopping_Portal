#include<iostream>
#include<string>
#include<cstdio>
#include<cstring>
#include<fstream>
#include<windows.h>
#include <conio.h>
#include"Cart.h"
#include<stdlib.h>
#include"Items.h"
using namespace std;
#include"Customer.h"

bool Customer::customer_Reg_Log_Menu()
{
label1:
	Customer cus;
	int choice;
	do
	{

		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
		cout << "\n\n";
		cout << "                                                    1)    Registration\n\n";
		cout << "                                                    2)    Login\n\n";
		cout << "                                                    3)    Go Back\n\n";
		cout << "                                      Enter Your Choice here                   ";
		while (!(cin >> choice) || choice < 1 || choice >3)
		{
			SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
			cout << "                Invalid input. Please enter an integer value in Given Range:   ";
			cin.clear();
			cin.ignore(MAXDWORD, '\n');
		}
		if (choice == 1)
		{
			cus.registraion();
		}
		if (choice == 2)
		{
			if (cus.login() == true)
			{
				system("cls");
				SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 15);
				cout << "\n\n                                                ---------------  LOGIN SUCCESFULLY  -------------\n\n";
				if (show_customer_menu() == false)
				{
					system("cls");
					goto label1;
				}
				break;
			}

			//                           ---------------------------------------------------------------------------
			else
			{
				int Num;
				cout << "\n\n                                  Do you Want To Forgot Password, PRESS 1 FOR [YES] AND 0 FOR [NO]                          ";
				while (!(cin >> Num) || Num < 0 || Num > 1)
				{
					SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
					cout << "                Invalid input. Please enter an integer value in Given Range:   ";
					cin.clear();
					cin.ignore(MAXDWORD, '\n');
				}
				if (Num == 1)
				{
				again32:
					string filename = "customer_account_save.txt", searchString, newValue, confirm_new_val;


					SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);

					cout << "\n\n                                                Enter Your User_Name                         ";
					cin >> searchString;
					cout << "\n\n                                                Enter Your New_Password                      ";
					cin >> newValue;
					cout << "\n\n                                                Enter Confirm New_Password                   ";
					cin >> confirm_new_val;

					if (newValue != confirm_new_val || newValue.length() < 6 || newValue.length() > 10 || newValue == " " || searchString == " ")
					{
						SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
						system("cls");

						cout << "\n\n\n ERROR REGISTRATION --> USER_NAME OR PASSWORD SHOULD NOT CONTAINS SPACES OR 7 CHARACTERS MINIMUM OR VERIFY YOUR CONFIRM PASSWORD\n\n";
						goto again32;
					}

					updateValueInFile(filename, searchString, newValue);
				}
				if (Num == 0)
				{
					goto label1;
				}




			}

			//                           ---------------------------------------------------------------------------

		}
		if (choice == 3)
		{
			return false;
		}
	} while (true);
}

void Customer::registraion()
{
	string str1;
	ofstream write_customer_account_reg;
	ifstream read_customer_account_reg;
	read_customer_account_reg.open("customer_account_save.txt");
	write_customer_account_reg.open("customer_account_save.txt", ios::app);
	system("cls");
again32:
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
	cout << "\n\n\n";
	cout << "                                                Enter Your User_Name                       ";
	cin >> reg_name;
	cout << "                                                Enter Your password                        ";
	cin >> reg_password;
	cout << "                                                Enter Confirm_Password                     ";
	cin >> confirm_pass;
	if (reg_password != confirm_pass || reg_name.length() < 6 || reg_name.length() > 10 || reg_password.length() < 6 || reg_password.length() > 10 || reg_password == " " || reg_name == " ")
	{
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
		system("cls");

		cout << "\n\n\n ERROR REGISTRATION --> USER_NAME OR PASSWORD SHOULD NOT CONTAINS SPACES OR 7 CHARACTERS MINIMUM OR VERIFY YOUR CONFIRM PASSWORD\n\n";
		goto again32;
	}
	if (read_customer_account_reg.is_open())
	{
		while (!read_customer_account_reg.eof())
		{
			getline(read_customer_account_reg, str1);
			if (str1 == reg_name)
			{
				SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
				system("cls");
				cout << "\n\n                     Account Already Registered, Youu Can Login By Your User_Name and password\n\n\n";
				return;
			}
		}
	}
	else
	{
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
		cout << "\n\n                      ******** FILE NOT FOUND *******  \n\n";
		return;
	}
	write_customer_account_reg << reg_name << endl;
	write_customer_account_reg << reg_password << endl;
	write_customer_account_reg.close();
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 15);
	system("cls");
	cout << "\n\n                                     ---------------  REGISTERED SUCCESFULLY  -------------\n\n";
	return;
}

bool Customer::login()
{
	string s1, s2;
	int choice;
	ifstream read_customer_account_log;
	read_customer_account_log.open("customer_account_save.txt");
	system("cls");
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
	if (read_customer_account_log.is_open())
	{
		cout << "\n\n                                                      Enter Your User_Name                   ";
		cin >> login_name;
		cout << "\n\n                                                      Enter Your password                    ";
		login_pass = getPasswordFromUser();
		cout << "\n\n";
		while (!read_customer_account_log.eof())
		{
			getline(read_customer_account_log, s1);
			getline(read_customer_account_log, s2);
			if (s1 == login_name && s2 == login_pass)
			{
				system("cls");
				SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 15);
				return true;
				read_customer_account_log.seekg(0, std::ios::beg);
				read_customer_account_log.close();
			}
		}
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
		system("cls");
		cout << "\n\n                                      NO REGISTRATION/ACCOUNT FOUND\n\n";


	}

	else
	{
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
		system("cls");
		cout << "\n\n                                      NO REGISTRATION/ACCOUNT FOUND\n\n";
		return false;
	}
}

string Customer::getPasswordFromUser()
{

	string password;
	char ch;
	while ((ch = _getch()) != '\r')
	{  // Read characters until Enter key is pressed
		if (ch == '\b')
		{  // Handle backspace
			if (!password.empty())
			{
				password.erase(password.size() - 1);  // Remove the last character from the string
				cout << "\b \b";  // Erase the last asterisk from the console
			}
		}
		else
		{
			password.push_back(ch);
			cout << '*';  // Display an asterisk for each character
		}
	}
	cout << endl;  // Move cursor to the next line
	return password;
}

// Customer Menu
bool Customer::show_customer_menu()
{
label:


	Item items;
	do
	{
		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
		cout << "\n\n                                            ####################  CUSTOMER VIEW ####################\n\n";
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 5);

		int choice;
		cout << "\n\n                                                 What Do You Want To Do? Choose From The Given Above:\n\n";
		cout << "                                                  1) Place An Order,     Press 1\n\n";
		cout << "                                                  2) Modify An Item,     Press 2\n\n";
		cout << "                                                  3) Display Cart Items, Press 3\n\n";
		cout << "                                                  4) Search An Item,     Press 4\n\n";
		cout << "                                                  5) Remove An Item,     Press 5\n\n";
		cout << "                                                  6) Show Total Bill,    Press 6\n\n";
		cout << "                                                  7) Go Back,            Press 7\n\n\n";
		cout << "                                           Enter Your Choice here                   ";
		while (!(cin >> choice) || choice < 1 || choice > 7)
		{
			SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
			cout << "                Invalid input. Please enter an integer value in Given Range:   ";
			cin.clear();
			cin.ignore(MAXDWORD, '\n');
		}
		if (choice == 1)
		{
			SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
			system("cls");
			if (items.print_Items_Menu() == false)
			{
				system("cls");
				goto label;

			}
			break;
		}
		if (choice == 2)
		{
			SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
			system("cls");
			break;
		}

		if (choice == 3)
		{
			SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
			system("cls");
			if (items.Display_Cart() == false)
			{
				system("cls");
				goto label;

			}
			break;
		}

		if (choice == 5)
		{
			SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
			system("cls");
			if (items.Item_Remove_from_Cart() == false)
			{
				system("cls");
				goto label;

			}
			break;
		}
		if (choice == 6)
		{
			SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
			system("cls");
			if (items.Bill() == false)
			{
				system("cls");
				goto label;

			}
			break;
		}


		if (choice == 7)
		{
			items.reset();
			return false;
		}

	} while (true);
}












//void Customer::display_Customer()
//{
//	string s1,s2;
//	ifstream read_customer_account_dis;
//
//	read_customer_account_dis.open("customer_account_save.txt");
//
//	if (read_customer_account_dis.is_open())
//	{
//
//		while (!read_customer_account_dis.eof())
//		{
//			getline(read_customer_account_dis, s1);
//			getline(read_customer_account_dis, s2);
//
//			cout << " User Name   " << s1 << "     Customer ID   "  << " \n\n";
//
//		}
//
//
//	}
//
//	else
//	{
//		cout << "\n\n File is Empty\n\n";
//		return;
//	}
//}





void Customer::updateValueInFile(const std::string& filename, const std::string& searchString, const std::string& newValue)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
	ifstream inputFile(filename);
	if (!inputFile)
	{
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
		cout << "\n\n                               Unable to open the file. \n\n";
		return;
	}

	std::ofstream tempFile("temp.txt");
	if (!tempFile) {
		std::cout << "Unable to create a temporary file." << std::endl;
		return;
	}

	std::string line;
	bool found = false;

	while (getline(inputFile, line)) {
		if (line == searchString) {
			found = true;
			tempFile << line << std::endl;
			std::getline(inputFile, line); // Skip the next line
			tempFile << newValue << std::endl;
		}
		else {
			tempFile << line << std::endl;
		}
	}

	inputFile.close();
	tempFile.close();

	if (!found)
	{
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
		system("cls");
		cout << "\n\n                                        User_Name not found in the file.\n\n";
		remove("temp.txt");
		return;
	}

	// Delete the existing file
	remove(filename.c_str());

	// Rename the temporary file
	if (rename("temp.txt", filename.c_str()) == 0)
	{
		system("cls");
		cout << "\n\n                                          PASSWORD UPDATED SUCCESSFULLY.\n\n";
	}
	else
	{
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
		system("cls");
		cout << "\n\n                                            Error renaming the file.\n\n";
	}
}