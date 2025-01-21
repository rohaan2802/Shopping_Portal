#pragma once
#include"Role.h"
#ifndef Customer_H
#define Customer_H

class Customer :protected Role
{
public:
	void registraion();
	bool login();
	string getPasswordFromUser();
	bool customer_Reg_Log_Menu();
	void updateValueInFile(const std::string& filename, const std::string& searchString, const std::string& newValue);
	bool show_customer_menu();
	//void display_Customer();
};
#endif // !Customer_H
