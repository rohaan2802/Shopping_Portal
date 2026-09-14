#pragma once
#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "Role.h"
#include <string>
using namespace std;

class Customer : protected Role
{
	string currentUser;
	string wishlist[100];
	int wishlist_size;

	void clearWishlistMemory();
	void loadWishlist();
	void saveWishlist();
public:
	Customer();
	void registraion() override;
	bool login() override;
	string getPasswordFromUser();
	bool customer_Reg_Log_Menu();
	void updateValueInFile(const string& filename, const string& searchString, const string& newValue);
	bool show_customer_menu();
	void forgotPassword();
	void checkoutAndSaveOrder();
	void viewOrderHistory();
	void manageWishlist();
};

#endif // !CUSTOMER_H
