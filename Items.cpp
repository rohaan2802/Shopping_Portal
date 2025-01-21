#include<iostream>
#include<fstream>
#include<iomanip>
#include"Customer.h"
#include<windows.h>
#include"Cart.h"
#include<string>
using namespace std;
#include"Items.h"
Cart c;


bool Item::print_Items_Menu()
{


	do
	{
		int choice;
		bool b = false;
		Item* items = new Item[500];
		int* arrayofcount = new int[100];
		int str_to_int, i_q, str_to_int1;
		ifstream read1;
		string filename;
		string  Item_Category[100];
		int itemcount = 1, i = 1, itemcount1 = 1, itemref;
		string str;

	label:
		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
		cout << "\n\n                                                ORDER PLACING\n\n";
		cout << "\n\n                                            Select The Category From The Given Below\n\n";
		ifstream read("ItemsCategory.txt");
		if (!read.is_open())
		{
			SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
			cout << "\n\n                      ******** FILE NOT FOUND *******  \n\n";
			return false;
		}
		while (!read.eof())
		{
			getline(read, str);
			Item_Category[itemcount] = str;
			if (itemcount < 10)
				cout << "                                            0" << itemcount << ")  " << str << endl;
			else
				cout << "                                            " << itemcount << ")  " << str << endl;
			arrayofcount[i++] = itemcount++;
		}
		read.close();
		cout << "\n\n                     " << " GO BACK, PRESS 0                    ";
		while (!(cin >> choice) || choice < 0 || choice >= itemcount)
		{
			SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
			cout << "                Invalid input. Please enter an integer value in Given Range:   ";
			cin.clear();
			cin.ignore(MAXDWORD, '\n');
		}
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 15);

		if (choice == 0)
		{
			return false;
		}


		for (int i = 1; i <= itemcount; i++)
		{
			if (choice == arrayofcount[i])
			{
				b = true;
				filename = Item_Category[i];
				read1.open(filename + ".txt");
				if (!read1.is_open())
				{
					SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
					system("cls");

					cout << "\n\n                      ******** FILE NOT FOUND ******* " << filename << "\n\n";
					goto label;
				}
			}


		}
		if (b == false)
		{
			SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
			system("cls");

			cout << "\n\n                      ******** FILE NOT FOUND ******* " << filename << "\n\n";
			goto label;
		}
		system("cls");
		cout << "\n\n";
		cout << left;
		cout << "                         " << setw(15) << "ITEM #" << "         " << setw(20) << "ITEM NAMES" << "             " << setw(28) << "ITEM PRICES" << setw(32) << "Items Quantity" << endl;
		cout << "\n\n";

		do
		{

			while (!read1.eof())
			{

				SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
				getline(read1, items[itemcount1].itemnames, '-');
				getline(read1, items[itemcount1].Items_Price, ',');
				getline(read1 >> ws, items[itemcount1].Items_Quantity);
				str_to_int = stoi(items[itemcount1].Items_Quantity);

				if (itemcount1 < 10)
					cout << setw(20) << "                          0" << itemcount1 << ")                    ";
				else
					cout << setw(20) << "                          " << itemcount1 << ")                    ";

				cout << setw(25) << items[itemcount1].itemnames << "          " << setw(33) << items[itemcount1].Items_Price;

				if (str_to_int < 10)
					cout << 0 << setw(22) << items[itemcount1].Items_Quantity << endl;
				else
					cout << setw(22) << items[itemcount1].Items_Quantity << endl;
				itemcount1++;

			}
			read1.close();
			SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
			cout << "\n\n                             Choose The item_# To Add The Item In Cart OR 	GO BACK, PRESS 0                     ";
			while (!(cin >> itemref) || itemref < 0 || itemref >= itemcount1)
			{
				SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
				cout << "                Invalid input. Please enter an integer value in Given Range:   ";
				cin.clear();
				cin.ignore(MAXDWORD, '\n');
			}
			SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 15);
			if (itemref == 0)
			{
				system("cls");
				break;
			}
			else
			{
				str_to_int1 = stoi(items[itemref].Items_Quantity);
				if (str_to_int1 == 0)
				{
					SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 1);
					cout << "\n     <<<<<<<<<<< Item Is Not Available  <<<<<<<<<<<                 \n";
					continue;
				}
				SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 1);
				cout << "\n     <<<<<<<<<<< Enter The Quantity  <<<<<<<<<<<                 ";
				while (!(cin >> i_q) || i_q <= 0 || i_q > str_to_int1)
				{
					SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
					cout << "                Invalid input. Please enter an integer value in Given Range:   ";
					cin.clear();
					cin.ignore(MAXDWORD, '\n');
				}
				str_to_int1 -= i_q;
				string new_val = to_string(str_to_int1);
				c.add_item(items[itemref].itemnames, items[itemref].Items_Price, i_q);

				update_items(filename, items[itemref].itemnames, items[itemref].Items_Price, new_val);

			}


		} while (true);


	} while (true);
	//delete[] items;
}



void Item::update_items(const std::string& filename, const std::string& name, string price, const std::string& newValue)
{
	Item* items = new Item[1000];
	int itemcount1 = 0;
	string file_name = filename + ".txt";
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
	ifstream inputFile(filename + ".txt");
	if (!inputFile)
	{
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
		cout << "\n\n                               Unable to open the file. \n\n";
		return;
	}

	ofstream tempFile("temp.txt");
	if (!tempFile) {
		std::cout << "Unable to create a temporary file." << std::endl;
		return;
	}

	while (getline(inputFile, items[itemcount1].itemnames, '-') &&
		getline(inputFile, items[itemcount1].Items_Price, ',') &&
		getline(inputFile >> ws, items[itemcount1].Items_Quantity))
	{

		if (items[itemcount1].itemnames == name && items[itemcount1].Items_Price == price)
		{
			tempFile << items[itemcount1].itemnames << '-' << items[itemcount1].Items_Price << ',' << ' ' << newValue;
		}
		else
		{
			tempFile << items[itemcount1].itemnames << '-' << items[itemcount1].Items_Price << ',' << ' ' << items[itemcount1].Items_Quantity;

		}

		// Check if it's the last line before writing a newline character
		if (!inputFile.eof())
		{
			tempFile << endl;
		}

		itemcount1++;
	}

	inputFile.close();
	tempFile.close();



	// Delete the existing file
	remove(file_name.c_str());

	// Rename the temporary file
	rename("temp.txt", file_name.c_str());

	delete[] items;
}






bool Item::Display_Cart()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
	int choice;
	c.DisplayItems();
	cout << "\n\n                     " << " GO BACK, PRESS 0                    ";
	while (!(cin >> choice) || choice < 0 || choice > 0)
	{
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
		cout << "                Invalid input. Please enter an integer value in Given Range:   ";
		cin.clear();
		cin.ignore(MAXDWORD, '\n');
	}
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 15);

	if (choice == 0)
	{
		return false;
	}

}



bool Item::Item_Remove_from_Cart()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
	int choice;
	c.DisplayItems();
	c.remove_item();

	cout << "\n\n                     " << " GO BACK, PRESS 0                    ";
	while (!(cin >> choice) || choice < 0 || choice > 0)
	{
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
		cout << "                Invalid input. Please enter an integer value in Given Range:   ";
		cin.clear();
		cin.ignore(MAXDWORD, '\n');
	}
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 15);

	if (choice == 0)
	{
		return false;
	}
}

bool Item::Bill()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
	int choice;
	c.DisplayItems();
	c.Bill();
next:
	cout << "\n\n                     " << " GO BACK, PRESS 0                    ";
	while (!(cin >> choice) || choice < 0 || choice > 0)
	{
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
		cout << "                Invalid input. Please enter an integer value in Given Range:   ";
		cin.clear();
		cin.ignore(MAXDWORD, '\n');
	}

	SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 15);

	if (choice == 0)
	{
		return false;
	}
}


void Item::reset()
{
	c.reset_data();

}








//void Item::update_items_remove(const std::string& name, string price, int& newValue)
//{
//
//	ifstream read("ItemsCategory.txt");
//	string str, str1;
//	string  Item_Category[100];
//	int itemcount = 1, itemcount1 = 1, i = 1;
//	ifstream read1;
//	string filename;
//	Item* items = new Item[500];
//	int str_to_int;
//	ofstream tempFile("temp.txt");
//	if (!tempFile) {
//		cout << "Unable to create a temporary file." << std::endl;
//		return;
//	}
//	while (!read.eof())
//	{
//		getline(read, str);
//		Item_Category[itemcount] = str;
//		itemcount++;
//	}
//	read.close();
//
//	for (int i = 1; i <= itemcount; i++)
//	{
//
//
//		filename = Item_Category[i];
//		read1.open(filename + ".txt");
//
//		do
//		{
//
//			while (!read1.eof())
//			{
//
//
//				getline(read1, items[itemcount1].itemnames, '-');
//				getline(read1, items[itemcount1].Items_Price, ',');
//				getline(read1 >> ws, items[itemcount1].Items_Quantity);
//				str_to_int = stoi(items[itemcount1].Items_Quantity);
//				if (items[itemcount1].itemnames == name && items[itemcount1].Items_Price == price)
//				{
//					items[itemcount1].Items_Quantity += newValue;
//					str1 = string(items[itemcount1].Items_Quantity);
//					tempFile << items[itemcount1].itemnames << '-' << items[itemcount1].Items_Price << ',' << ' ' << str1;
//
//				}
//				else
//				{
//					tempFile << items[itemcount1].itemnames << '-' << items[itemcount1].Items_Price << ',' << ' ' << items[itemcount1].Items_Quantity;
//
//				}
//
//				// Check if it's the last line before writing a newline character
//				if (!read1.eof())
//				{
//					tempFile << endl;
//				}
//				itemcount1++;
//
//			}
//			read1.close();
//
//		} while (true);
//
//
//	}
//
//
//
//
//
//
//}