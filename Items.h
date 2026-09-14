#pragma once
#ifndef ITEMS_H
#define ITEMS_H

#include <string>
using namespace std;

class Item
{
public:
	string itemnames = "";
	string Items_Price = "";
	string Items_Quantity = "";

	bool print_Items_Menu();
	bool Display_Cart();
	bool Item_Remove_from_Cart();
	bool Modify_Cart_Item();
	bool Search_Catalog();
	bool Bill();
	void reset();
	void update_items(const string& filename, const string& name, string price, const string& newValue);

	// Catalog management used by Admin / Vendor
	static bool listCategories();
	static bool addCategory(const string& name);
	static bool removeCategory(const string& name);
	static bool addProduct(const string& category, const string& name, const string& priceLabel, int qty);
	static bool updateProduct(const string& category, const string& name, const string& newPriceLabel, int newQty);
	static bool removeProduct(const string& category, const string& name);
	static bool viewCategoryProducts(const string& category);
	static bool searchAllCatalogs(const string& query);

	/** True if trimmed name appears as a non-empty line in ItemsCategory.txt. */
	static bool categoryExists(const string& name);
	/** Create empty category .txt if missing; returns false if cannot create. */
	static bool ensureCategoryFile(const string& name);
	/** Fill out[] with category names from ItemsCategory.txt; returns count (capped at maxCount). */
	static int listCategoriesFromFile(string* out, int maxCount);
	/** True if category .txt exists and contains at least one product line. */
	static bool categoryHasProducts(const string& name);
};

#endif // !ITEMS_H
