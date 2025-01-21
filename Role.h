#pragma once
#include<iostream>
using namespace std;
#ifndef Role_H
#define Role_H





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
		login_name = "";
		login_pass = "";
	}
	virtual void registraion() = 0;
	virtual bool login() = 0;



};




#endif // !ROLE_H
