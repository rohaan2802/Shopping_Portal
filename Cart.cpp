#include<iostream>
#include<Windows.h>
#include<fstream>
#include<iomanip>
#include<string>
#include"Items.h"
#include <cctype>
using namespace std;
#include"Cart.h"

void Cart::add_item(string i_n, string i_p, int i_q)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	int index = -1;
	for (int i = 0; i < cart_size; i++)
	{
		if (itemnames[i] == i_n)
		{
			index = i;
			break;
		}
	}

	if (index == -1)
	{
		itemnames[cart_size] = i_n;
		Items_Price[cart_size] = i_p;
		Items_Quantity[cart_size] = i_q;
		cart_size++;
	}
	else
	{
		Items_Quantity[index] += i_q;
	}


	SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 1);
	cout << "\n  <<<<<<<<<   Item(s) Added to Cart Successfuly!    <<<<<<<<<";
}


int Cart::cart_size = 1;

void Cart::reset_data()
{
	memset(&cart_size, 0, sizeof(cart_size));
}

void Cart::DisplayItems()
{
	double pr = 0;
	if (cart_size > 1)
	{
		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
		cout << "\n\n";
		cout << "                                           <<<<<<<<<<<<<<<<<<<  DISPLAYING ITEMS IN CART   <<<<<<<<<<<<<<<<<<<\n\n";
		cout << left;
		//cout << "                         " << setw(20) << "ITEM NAMES" << "                        " << setw(28) << "ITEM PRICES" << setw(32) << "Items Quantity" << "\n\n";
		cout << "                         " << setw(15) << "ITEM #" << "         " << setw(20) << "ITEM NAMES" << "             " << setw(28) << "ITEM PRICES" << setw(32) << "Items Quantity" << endl;

		/*for (int i = 1; i < cart_size; i++)
		{

			cout << setw(25) << "                  " << setw(25) << itemnames[i] << "                    " << setw(34) << Items_Price[i];

			if (Items_Quantity[i] < 10)
				cout << 0 << setw(15) << Items_Quantity[i] << endl;
			else
				cout << setw(15) << Items_Quantity[i] << endl;
		}*/
		string numeric_part;

		for (int i = 1; i < cart_size; i++)
		{
			for (char c : Items_Price[i]) {
				if (isdigit(c) || c == '.')
				{
					numeric_part += c;
				}
			}
			pr += (stoi(numeric_part)) * Items_Quantity[i];

			if (cart_size < 10)
				cout << setw(20) << "                          0" << i << ")                    ";
			else
				cout << setw(20) << "                          " << i << ")                    ";

			cout << setw(25) << itemnames[i] << "          Rs. " << setw(30) << pr;

			if (Items_Quantity[i] < 10)
				cout << 0 << setw(22) << Items_Quantity[i] << endl;
			else
				cout << setw(22) << Items_Quantity[i] << endl;
		}
		cout << "\n\n";
	}
	else
	{
		cout << "\n\n";
		cout << "                                           <<<<<<<<<<<<<<<<<<<  CART IS EMPTY   <<<<<<<<<<<<<<<<<<<\n\n";
	}
}




void Cart::remove_item()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	int index;
	Cart c1;
	if (cart_size > 1)
	{
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
		cout << "\n\n                             Choose The item_# To Remove The Item From Cart                       ";
		while (!(cin >> index) || index < 1 || index >= cart_size)
		{
			SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);
			cout << "                            Invalid input. Please enter an integer value in Given Range:           ";
			cin.clear();
			cin.ignore(MAXDWORD, '\n');
		}



		///////////////////////////////////////////////////////////////////////
		ifstream read("ItemsCategory.txt");
		string str, str1;
		string  Item_Category[100];
		int itemcount = 1, itemcount1 = 1, i = 1;
		ifstream read1;
		string filename;
		//	Item* items = new Item[500];
		string* n = new string[500];
		string* p = new string[500];
		string* q = new string[500];
		int str_to_int;
		ofstream tempFile("temp.txt");
		if (!tempFile) {
			cout << "Unable to create a temporary file." << std::endl;
			return;
		}
		while (!read.eof())
		{
			getline(read, str);
			Item_Category[itemcount] = str;
			itemcount++;
		}
		read.close();

		for (int i = 1; i <= itemcount; i++)
		{


			filename = Item_Category[i];
			read1.open(filename + ".txt");



			while (!read1.eof())
			{


				getline(read1, n[itemcount1], '-');
				getline(read1, p[itemcount1], ',');
				getline(read1 >> ws, q[itemcount1]);
				str_to_int = stoi(q[itemcount1]);
				if (n[itemcount1] == itemnames[index] && p[itemcount1] == Items_Price[index])
				{
					int x = stoi(q[itemcount1]);
					x += Items_Quantity[index];
					str1 = to_string(x);
					tempFile << n[itemcount1] << '-' << p[itemcount1] << ',' << ' ' << str1;

				}
				else
				{
					tempFile << n[itemcount1] << '-' << p[itemcount1] << ',' << ' ' << q[itemcount1];

				}

				// Check if it's the last line before writing a newline character
				if (!read1.eof())
				{
					tempFile << endl;
				}
				itemcount1++;

			}




		}

		read1.close();

		tempFile.close();



		// Delete the existing file
		remove(filename.c_str());

		// Rename the temporary file
		rename("temp.txt", filename.c_str());


















		//////////////////////////////////////////////////////////////////////////////////////////////////////////

		for (int i = index; i < cart_size - 1; i++)
		{
			itemnames[i] = itemnames[i + 1];
			Items_Price[i] = Items_Price[i + 1];
			Items_Quantity[i] = Items_Quantity[i + 1];
		}
		cart_size--;
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 1);
		cout << "\n\n";
		cout << "           <<<<<<<<<<<<<<<<<<<  ITEMS(S) REMOVED SUCCESSFULY  <<<<<<<<<<<<<<<<<<<\n\n";

	}

}


bool Cart::Bill()
{
	if (cart_size > 1)
	{



		// Remove the currency symbol and whitespace from the string
		string numeric_part;
		for (int i = 1; i < cart_size; i++)
		{
			for (char c : Items_Price[i]) {
				if (isdigit(c) || c == '.')
				{
					numeric_part += c;
				}
			}
			total_bill += (stoi(numeric_part)) * Items_Quantity[i];

		}


		cout << "                                                                                            -----------------------------------\n";
		cout << "\n\n  <<<<<<<< Delivery Charges                                                                      " << (total_bill * 5) / 100 << endl;
		cout << "                                                                                            -----------------------------------\n";
		cout << "\n\n  <<<<<<<< TOTAL AMOUNT                                                                      " << total_bill << endl;


	}
	else
	{
		return false;
		/* cout << "\n\n";
		 cout << "                                           <<<<<<<<<<<<<<<<<<<  CART IS EMPTY   <<<<<<<<<<<<<<<<<<<\n\n";*/
	}
	return true;
}







