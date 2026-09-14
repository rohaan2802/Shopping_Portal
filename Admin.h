#pragma once
#ifndef ADMIN_H
#define ADMIN_H

#include "Role.h"
#include <string>
using namespace std;

class Admin : public Role
{
	string currentUser;
public:
	Admin();
	void registraion() override; // seed default admin if missing
	bool login() override;
	bool admin_menu();
	void viewUsers();
	void manageCategories();
	void manageProducts();
	void viewOrdersAndStats();
	static void ensureDefaultAdmin();
};

#endif // !ADMIN_H
