// Lightweight unit checks for Cart merge + addProduct duplicate rejection.
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include "../Cart.h"
#include "../Items.h"

using namespace std;

static int g_fail = 0;

static void expect(bool cond, const string& msg)
{
	if (!cond)
	{
		cerr << "FAIL: " << msg << "\n";
		g_fail++;
	}
	else
		cout << "PASS: " << msg << "\n";
}

int main()
{
	{
		Cart cart;
		cart.add_item("Yoga mat ", " 300 PKR", 1);
		cart.add_item("Yoga mat ", " 300 PKR", 2);
		cart.add_item("Yoga mat ", " 1500 PKR", 1);
		expect(cart.size() == 2, "cart merges only identical name+price");
		expect(cart.getQty(0) == 3, "merged quantity is 3");
		expect(cart.getQty(1) == 1, "different price stays separate");
		cart.reset_data();
		expect(cart.isEmpty(), "reset clears cart");
	}

	{
		ofstream f("UnitTestCat.txt");
		f << "Alpha - 10 PKR, 2\n";
		f.close();
		bool okAdd = Item::addProduct("UnitTestCat", "Beta", "15 PKR", 4);
		bool dup = Item::addProduct("UnitTestCat", "Alpha", "10 PKR", 9);
		bool dup2 = Item::addProduct("UnitTestCat", "Beta", "15 PKR", 1);
		expect(okAdd, "addProduct accepts new name");
		expect(!dup, "addProduct rejects existing Alpha");
		expect(!dup2, "addProduct rejects existing Beta");
		remove("UnitTestCat.txt");
	}

	return g_fail ? 1 : 0;
}
