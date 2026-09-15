#pragma once
#ifndef ADMIN_H
#define ADMIN_H

#include "Role.h"
#include <string>
using namespace std;

/**
 * Admin role: only one admin account may be registered (stored in admin_accounts.txt).
 * Portal entry: Registration / Login / Back (same as Customer/Vendor).
 */
class Admin : public Role
{
	string currentUser;

	static bool hasRegisteredAdmin();
	static int countAdmins();

public:
	Admin();
	void registraion() override; /* create the single admin if none exists */
	bool login() override;
	bool admin_Reg_Log_Menu(); /* 1 Register, 2 Login, 3 Back */
	bool admin_menu();
	void viewUsers();
	void manageCategories();
	void manageProducts();
	void viewOrdersAndStats();
};

#endif // !ADMIN_H
