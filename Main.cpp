#include<iostream>
#include<Windows.h>
using namespace std;
#include"Role.h"
#include"Admin.h"
#include"Vendor.h"
#include"Customer.h"
#include"Items.h"
#include"Cart.h"
#include"Other_Fun.h"







int main()
{
back:
	HWND consoleHandle = GetConsoleWindow();
	//Set the window state to maximized
	ShowWindow(consoleHandle, SW_MAXIMIZE);


	Welcome_Message();


	int select_menu = Select_Role();
	system("CLS");
	Customer cus;
	if (select_menu == 3)
	{
		if (cus.customer_Reg_Log_Menu() == false)
		{
			goto back;
		}
	}

	if (select_menu == 4)
	{
		HWND consoleWindow = GetConsoleWindow();
		PostMessage(consoleWindow, WM_CLOSE, 0, 0);
	}

	system("pause");
	return 0;
}













