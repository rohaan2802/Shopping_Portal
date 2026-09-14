#pragma once
#ifndef VENDOR_H
#define VENDOR_H

#include "Role.h"
#include <string>
using namespace std;

class Vendor : public Role
{
	string company;
	string currentUser;
public:
	Vendor();
	void registraion() override;
	bool login() override;
	bool vendor_menu();
	void manageOwnStock();
	void viewOwnProducts();
};

#endif // !VENDOR_H
