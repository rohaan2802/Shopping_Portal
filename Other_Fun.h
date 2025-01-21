#pragma once
#include<iostream>
#include<windows.h>
#include<limits>
#include<conio.h>
#include"Customer.h"
using namespace std;
#ifndef Other_Fun_H
#define Other_Fun_H

void Welcome_Message()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_FONT_INFOEX fontInfo;
	fontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(consoleHandle, FALSE, &fontInfo);
	// Modify the font size and intensity
	fontInfo.dwFontSize.X = 0; // Width of the font, set to 0 for default
	fontInfo.dwFontSize.Y = 22; // Height of the font
	fontInfo.FontWeight = FW_BOLD; // Font intensity
	SetCurrentConsoleFontEx(consoleHandle, FALSE, &fontInfo);
	//SetConsoleTextAttribute(consoleHandle, 20);
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 3);
	// Clear the console screen
	system("cls");
	cout << "\n\n\n               ************************************  WELCOME TO FAST SHOPPING PORTAL  ************************************\n\n\n";
}

int Select_Role()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 5);
	int Role_Selection;
	cout << "\n\n                                                         What Is Your Login Type....\n\n";
	cout << "                                                               1. Admin\n";
	cout << "                                                               2. Vendor\n";
	cout << "                                                               3. Customer\n";
	cout << "                                                               4. GO BACK & CLOSE THE WINDOW, PRESS 4\n\n";
	cout << "                                      Enter Your Choice here                   ";
	while (!(cin >> Role_Selection) || Role_Selection < 1 || Role_Selection >4)
	{
		SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 4);

		cout << "                Invalid input. Please enter an integer value in Given Range:   ";
		cin.clear();
		cin.ignore(MAXDWORD, '\n');
	}
	return Role_Selection;
	SetConsoleTextAttribute(consoleHandle, BACKGROUND_INTENSITY | 0);
}

#endif   //!Other_Fun_H


//0 = Black
//1 = Blue
//2 = Green
//3 = Cyan
//4 = Red
//5 = Magenta
//6 = Yellow
//7 = White
//8 = Gray
//9 = Light Blue
//10 = Light Green
//11 = Light Cyan
//12 = Light Red
//13 = Light Magenta
//14 = Light Yellow
//15 = Bright White


/*Background Color Constants :
	---------------------------- -
		BACKGROUND_BLUE - Blue background
		BACKGROUND_GREEN - Green background
		BACKGROUND_RED - Red background
		BACKGROUND_INTENSITY - Intense background color(bright)
		BACKGROUND_CYAN - Cyan(blue - green) background
		BACKGROUND_MAGENTA - Magenta(purplish - red) background
		BACKGROUND_YELLOW - Yellow background
		BACKGROUND_WHITE - White background(gray on some systems)*/