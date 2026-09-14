#pragma once
#ifndef ROLE_H
#define ROLE_H

#include <string>
using namespace std;

class Role
{
protected:
	string reg_name;
	string reg_password;
	string confirm_pass;
	string login_name;
	string login_pass;
public:
	Role()
	{
		reg_name = "";
		reg_password = "";
		confirm_pass = "";
		login_name = "";
		login_pass = "";
	}
	virtual void registraion() = 0;
	virtual bool login() = 0;
	virtual ~Role() {}
};

#endif // !ROLE_H
