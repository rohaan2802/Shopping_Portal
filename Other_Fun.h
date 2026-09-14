#pragma once
#ifndef OTHER_FUN_H
#define OTHER_FUN_H

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _HAS_STD_BYTE
#define _HAS_STD_BYTE 0
#endif

#include <iostream>
#include <limits>
#include <string>
#include <cctype>
#include <windows.h>
#include <conio.h>

using namespace std;

// Console color helpers (Other_Fun style — spacious colorful UI)
// 0 Black  1 Blue  2 Green  3 Cyan  4 Red  5 Magenta  6 Yellow  7 White
// 8 Gray   9 LBlue 10 LGreen 11 LCyan 12 LRed 13 LMagenta 14 LYellow 15 BrightWhite

inline HANDLE console()
{
	return GetStdHandle(STD_OUTPUT_HANDLE);
}

inline void setColor(int n)
{
	SetConsoleTextAttribute(console(), BACKGROUND_INTENSITY | n);
}

inline void setFontBold(int height = 22)
{
	CONSOLE_FONT_INFOEX fontInfo;
	fontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(console(), FALSE, &fontInfo);
	fontInfo.dwFontSize.X = 0;
	fontInfo.dwFontSize.Y = height;
	fontInfo.FontWeight = FW_BOLD;
	SetCurrentConsoleFontEx(console(), FALSE, &fontInfo);
}

inline void clearScreen()
{
	system("cls");
}

inline void pauseEnter()
{
	setColor(8);
	cout << "\n\n                         Press Enter to continue...";
	cin.clear();
	cin.ignore((numeric_limits<streamsize>::max)(), '\n');
	string dummy;
	getline(cin, dummy);
}

inline void banner(const string& title, int color = 3)
{
	setColor(color);
	cout << "\n\n";
	cout << "               **********************************************************************************************\n";
	cout << "                                      " << title << "\n";
	cout << "               **********************************************************************************************\n\n\n";
}

inline void sectionTitle(const string& title, int color = 5)
{
	setColor(color);
	cout << "\n\n                         ####################  " << title << "  ####################\n\n";
	setColor(0);
}

inline void successMsg(const string& msg)
{
	setColor(15);
	cout << "\n\n                         ---------------  " << msg << "  ---------------\n\n";
}

inline void errorMsg(const string& msg)
{
	setColor(4);
	cout << "\n\n                         *** " << msg << " ***\n\n";
	setColor(0);
}

inline void infoMsg(const string& msg)
{
	setColor(1);
	cout << "\n                         <<<<<<<<<  " << msg << "  <<<<<<<<<\n";
	setColor(0);
}

inline int readIntInRange(const string& prompt, int lo, int hi)
{
	int value;
	setColor(0);
	cout << prompt;
	while (!(cin >> value) || value < lo || value > hi)
	{
		setColor(4);
		cout << "                         Invalid input. Enter an integer in range [" << lo << " - " << hi << "]:   ";
		cin.clear();
		cin.ignore(MAXDWORD, '\n');
		setColor(0);
	}
	cin.ignore((numeric_limits<streamsize>::max)(), '\n');
	return value;
}

inline string readLine(const string& prompt)
{
	string s;
	setColor(0);
	cout << prompt;
	getline(cin, s);
	return s;
}

inline string readToken(const string& prompt)
{
	string s;
	setColor(0);
	cout << prompt;
	cin >> s;
	cin.ignore((numeric_limits<streamsize>::max)(), '\n');
	return s;
}

inline string getPasswordMasked()
{
	string password;
	char ch;
	while ((ch = static_cast<char>(_getch())) != '\r')
	{
		if (ch == '\b')
		{
			if (!password.empty())
			{
				password.pop_back();
				cout << "\b \b";
			}
		}
		else if (ch >= 32 && ch <= 126)
		{
			password.push_back(ch);
			cout << '*';
		}
	}
	cout << endl;
	return password;
}

inline bool validCredential(const string& user, const string& pass, const string& confirm = "")
{
	if (user.length() < 6 || user.length() > 16)
		return false;
	if (pass.length() < 6 || pass.length() > 16)
		return false;
	for (char c : user)
		if (isspace(static_cast<unsigned char>(c)))
			return false;
	for (char c : pass)
		if (isspace(static_cast<unsigned char>(c)))
			return false;
	if (!confirm.empty() && pass != confirm)
		return false;
	return true;
}

inline string extractPriceNumber(const string& priceStr)
{
	string numeric;
	for (char c : priceStr)
	{
		if (isdigit(static_cast<unsigned char>(c)) || c == '.')
			numeric += c;
	}
	return numeric.empty() ? "0" : numeric;
}

inline void Welcome_Message()
{
	setFontBold(22);
	setColor(3);
	clearScreen();
	cout << "\n\n\n";
	cout << "               ************************************  WELCOME TO FAST SHOPPING PORTAL  ************************************\n";
	cout << "\n";
	cout << "                                   Author: Mohammad Rohaan  |  22I-2327  |  Sec-Z\n";
	cout << "                                   OOP Console Shopping Portal — Admin · Vendor · Customer\n\n\n";
}

inline int Select_Role()
{
	setColor(5);
	cout << "\n\n                                                         What Is Your Login Type....\n\n";
	cout << "                                                               1. Admin\n\n";
	cout << "                                                               2. Vendor\n\n";
	cout << "                                                               3. Customer\n\n";
	cout << "                                                               4. Exit / Close Window\n\n";
	return readIntInRange("                                      Enter Your Choice here                   ", 1, 4);
}

#endif // !OTHER_FUN_H
