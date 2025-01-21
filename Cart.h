#pragma once
#include<iostream>
#include"Items.h"
using namespace std;
#ifndef Cart_H
#define Cart_H

class Cart
{
	static int cart_size;
	string* itemnames = new string[500];
	string* Items_Price = new string[500];
	int* Items_Quantity = new int[500];
	double total_bill;
public:
	void add_item(string, string, int);
	void DisplayItems();
	bool Bill();
	void Search();
	void remove_item();
	void reset_data();
	//	void update_items_remove(const std::string& name, string price, int& newValue);


		//void update_items(const std::string& filename, const std::string& name, string price, const std::string& newValue);

		/*~Cart()
		{
			delete[] my_cart;
		}*/

};


#endif // !Cart_H
