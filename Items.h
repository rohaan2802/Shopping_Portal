#pragma once
#include<string>
#include<iostream>
using namespace std;
#ifndef Items_H
#define Items_H
#include"Cart.h"

class Item
{
public:
	//	int     Items_Count = 0;
	string  itemnames = "";
	string  Items_Price = "";
	string  Items_Quantity = "";
	//	virtual	void Search();
	bool print_Items_Menu();
	bool Display_Cart();
	bool Item_Remove_from_Cart();
	bool Bill();
	void reset();
	void update_items(const std::string& filename, const std::string& name, string price, const std::string& newValue);
	void update_items_remove(const std::string& name, string price, int& newValue);

};

#endif // !Items_H
