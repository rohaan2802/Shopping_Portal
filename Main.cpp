#include <iostream>
#include <windows.h>
#include "Role.h"
#include "Admin.h"
#include "Vendor.h"
#include "Customer.h"
#include "Items.h"
#include "Cart.h"
#include "Other_Fun.h"

using namespace std;

int main()
{
back:
	HWND consoleHandle = GetConsoleWindow();
	ShowWindow(consoleHandle, SW_MAXIMIZE);

	Welcome_Message();
	int select_menu = Select_Role();
	clearScreen();

	if (select_menu == 1)
	{
		Admin admin;
		if (admin.login())
			admin.admin_menu();
		else
			pauseEnter();
		goto back;
	}
	else if (select_menu == 2)
	{
		Vendor vendor;
		clearScreen();
		banner("VENDOR PORTAL", 5);
		cout << "                         1) Register\n\n";
		cout << "                         2) Login\n\n";
		cout << "                         3) Back\n\n";
		int vch = readIntInRange("                         Choice:   ", 1, 3);
		if (vch == 1)
		{
			vendor.registraion();
			pauseEnter();
		}
		else if (vch == 2)
		{
			if (vendor.login())
				vendor.vendor_menu();
			else
				pauseEnter();
		}
		goto back;
	}
	else if (select_menu == 3)
	{
		Customer cus;
		if (cus.customer_Reg_Log_Menu() == false)
			goto back;
	}
	else if (select_menu == 4)
	{
		setColor(3);
		cout << "\n\n                         Thank you for visiting FAST Shopping Portal!\n\n";
		HWND consoleWindow = GetConsoleWindow();
		PostMessage(consoleWindow, WM_CLOSE, 0, 0);
		return 0;
	}

	system("pause");
	return 0;
}
