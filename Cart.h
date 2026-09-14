#pragma once
#ifndef CART_H
#define CART_H

#include <string>
using namespace std;

class Cart
{
	int cart_size; // number of items currently in cart (0 = empty)
	string itemnames[500];
	string Items_Price[500];
	int Items_Quantity[500];
	double total_bill;
public:
	Cart();
	void add_item(string name, string price, int qty);
	void DisplayItems();
	bool Bill(double taxPercent = 5.0, double deliveryPercent = 5.0);
	void Search(const string& query);
	bool modify_quantity();
	void remove_item();
	void reset_data();
	void restock_all_and_clear(); // restore catalog stock then empty cart (logout / abandon)
	int size() const { return cart_size; }
	bool isEmpty() const { return cart_size <= 0; }
	string getName(int i) const { return itemnames[i]; }
	string getPrice(int i) const { return Items_Price[i]; }
	int getQty(int i) const { return Items_Quantity[i]; }
	double lastBillTotal() const { return total_bill; }
};

#endif // !CART_H
