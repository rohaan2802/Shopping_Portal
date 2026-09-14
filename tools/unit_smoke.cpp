// Lightweight unit checks for Cart merge + category-gated product CRUD.
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include "../Cart.h"
#include "../Items.h"
#include "../Other_Fun.h"

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

static string readFile(const string& path)
{
	ifstream in(path);
	if (!in) return "";
	ostringstream ss;
	ss << in.rdbuf();
	return ss.str();
}

static void writeFile(const string& path, const string& body)
{
	ofstream out(path, ios::trunc);
	out << body;
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
		// Stock-at-checkout helpers: write a tiny catalog and deduct via validateAndDeductStock
		string oldCats = readFile("ItemsCategory.txt");

		writeFile("ItemsCategory.txt", "UnitStockCat\n");
		writeFile("UnitStockCat.txt", "Widget - 10 PKR, 5\n");

		Cart cart;
		cart.add_item("Widget", " 10 PKR", 2);
		expect(Cart::catalogStock("Widget", " 10 PKR") == 5, "catalog stock unread until deduct");
		expect(cart.validateAndDeductStock(), "checkout deduct succeeds with enough stock");
		expect(Cart::catalogStock("Widget", " 10 PKR") == 3, "stock reduced only after deduct");

		Cart cart2;
		cart2.add_item("Widget", " 10 PKR", 9);
		expect(!cart2.validateAndDeductStock(), "checkout rejects when stock insufficient");
		expect(Cart::catalogStock("Widget", " 10 PKR") == 3, "failed checkout leaves stock unchanged");

		remove("UnitStockCat.txt");
		if (oldCats.empty())
			remove("ItemsCategory.txt");
		else
			writeFile("ItemsCategory.txt", oldCats);
	}

	{
		string oldCats = readFile("ItemsCategory.txt");

		// Fake category (not in ItemsCategory.txt) must be rejected even if a .txt exists
		writeFile("FakeUnitCat.txt", "Ghost - 1 PKR, 1\n");
		expect(!Item::categoryExists("FakeUnitCat"), "categoryExists false for unlisted category");
		expect(!Item::addProduct("FakeUnitCat", "Ghost2", "2 PKR", 1), "addProduct rejects fake category");
		expect(!Item::updateProduct("FakeUnitCat", "Ghost", "9 PKR", 2), "updateProduct rejects fake category");
		expect(!Item::removeProduct("FakeUnitCat", "Ghost"), "removeProduct rejects fake category");
		expect(!Item::viewCategoryProducts("FakeUnitCat"), "viewCategoryProducts rejects fake category");
		remove("FakeUnitCat.txt");

		// Valid registered category: duplicate rejection still works
		writeFile("ItemsCategory.txt", "UnitTestCat\n");
		writeFile("UnitTestCat.txt", "Alpha - 10 PKR, 2\n");
		expect(Item::categoryExists("UnitTestCat"), "categoryExists true for listed category");
		bool okAdd = Item::addProduct("UnitTestCat", "Beta", "15 PKR", 4);
		bool dup = Item::addProduct("UnitTestCat", "Alpha", "10 PKR", 9);
		bool dup2 = Item::addProduct("UnitTestCat", "Beta", "15 PKR", 1);
		expect(okAdd, "addProduct accepts new name on valid category");
		expect(!dup, "addProduct rejects existing Alpha");
		expect(!dup2, "addProduct rejects existing Beta");

		// Missing .txt for a listed category should also reject product CRUD
		writeFile("ItemsCategory.txt", "MissingFileCat\n");
		remove("MissingFileCat.txt");
		expect(Item::categoryExists("MissingFileCat"), "listed category exists in index");
		expect(!Item::addProduct("MissingFileCat", "X", "1 PKR", 1), "addProduct rejects missing category file");

		// removeCategory blocks when products exist
		writeFile("ItemsCategory.txt", "UnitBlockCat\n");
		writeFile("UnitBlockCat.txt", "KeepMe - 5 PKR, 3\n");
		expect(Item::categoryHasProducts("UnitBlockCat"), "categoryHasProducts detects products");
		expect(!Item::removeCategory("UnitBlockCat"), "removeCategory blocked when products exist");
		expect(Item::categoryExists("UnitBlockCat"), "blocked delete leaves category listed");

		// Empty category file can be removed
		writeFile("UnitBlockCat.txt", "");
		expect(!Item::categoryHasProducts("UnitBlockCat"), "empty file has no products");
		expect(Item::removeCategory("UnitBlockCat"), "removeCategory allowed for empty category");
		expect(!Item::categoryExists("UnitBlockCat"), "category removed from index");

		remove("UnitTestCat.txt");
		remove("MissingFileCat.txt");
		remove("UnitBlockCat.txt");
		if (oldCats.empty())
			remove("ItemsCategory.txt");
		else
			writeFile("ItemsCategory.txt", oldCats);
	}

	return g_fail ? 1 : 0;
}
