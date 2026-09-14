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
#include <iomanip>
#include <sstream>
#include <limits>
#include <string>
#include <cctype>
#include <cstdlib>
#include <io.h>
#include <stdio.h>
#include <windows.h>
#include <conio.h>

using namespace std;

/* ---------- Dark theme (black background, high-contrast foreground) ---------- */
/* 0 Black  1 Blue  2 Green  3 Cyan  4 Red  5 Magenta  6 Brown/Yellow  7 LightGray */
/* 8 DarkGray  9 LBlue  10 LGreen  11 LCyan  12 LRed  13 LMagenta  14 LYellow  15 White */

inline HANDLE console()
{
	return GetStdHandle(STD_OUTPUT_HANDLE);
}

/** Right-pad Sr# / ITEM # so values < 10 keep a leading space (columns stay aligned). */
inline void printPaddedIndex(ostream& out, int n, int width = 2)
{
	/* Build string first so sticky left/right flags on 'out' cannot break padding. */
	ostringstream oss;
	oss << right << setfill(' ') << setw(width) << n;
	out << oss.str();
}

inline string paddedIndex(int n, int width = 2)
{
	ostringstream oss;
	oss << right << setfill(' ') << setw(width) << n;
	return oss.str();
}

/** Standard list line: "  1)  Name" / " 10)  Name" with aligned ')' and text. */
inline void printNumberedLine(ostream& out, int n, const string& text, int width = 2)
{
	out << "                         ";
	printPaddedIndex(out, n, width);
	out << ")  " << text << "\n";
}

inline void setColor(int fg)
{
	/* Dark theme: never draw black-on-black; bump classic 1-6 to bright 9-14 */
	if (fg == 0)
		fg = 7;
	else if (fg >= 1 && fg <= 6)
		fg += 8;
	SetConsoleTextAttribute(console(), static_cast<WORD>(fg & 0x0F));
}

inline void setDefaultColor()
{
	setColor(7); /* light gray on black */
}

inline void setFontBold(int height = 24)
{
	CONSOLE_FONT_INFOEX fontInfo;
	fontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(console(), FALSE, &fontInfo);
	fontInfo.dwFontSize.X = 0;
	fontInfo.dwFontSize.Y = height;
	fontInfo.FontWeight = FW_BOLD;
	wcscpy_s(fontInfo.FaceName, L"Consolas");
	SetCurrentConsoleFontEx(console(), FALSE, &fontInfo);
}

inline void enableDarkTheme()
{
	/* Force black background + bright white default (cmd color table) */
	system("color 0F");
	SetConsoleOutputCP(437); /* OEM USA - ASCII-safe, avoids mojibake */
	SetConsoleCP(437);
	setDefaultColor();
	setFontBold();
}

inline string trimCopy(string s)
{
	while (!s.empty() && isspace(static_cast<unsigned char>(s.front())))
		s.erase(s.begin());
	while (!s.empty() && isspace(static_cast<unsigned char>(s.back())))
		s.pop_back();
	return s;
}

inline bool parseIntSafe(const string& s, int& out)
{
	try
	{
		size_t idx = 0;
		string t = trimCopy(s);
		if (t.empty()) return false;
		out = stoi(t, &idx);
		return idx > 0;
	}
	catch (...)
	{
		return false;
	}
}

inline bool parseDoubleSafe(const string& s, double& out)
{
	try
	{
		size_t idx = 0;
		string t = trimCopy(s);
		if (t.empty()) return false;
		out = stod(t, &idx);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

inline void clearScreen()
{
	system("cls");
	setDefaultColor();
}

inline void pauseEnter()
{
	setColor(8);
	cout << "\n\n                         Press Enter to continue...";
	setDefaultColor();
	cin.clear();
	string dummy;
	getline(cin, dummy);
}

inline void banner(const string& title, int color = 11)
{
	setColor(color);
	cout << "\n\n";
	cout << "               ==================================================================================================\n";
	cout << "                                      " << title << "\n";
	cout << "               ==================================================================================================\n\n\n";
	setDefaultColor();
}

inline void sectionTitle(const string& title, int color = 14)
{
	setColor(color);
	cout << "\n\n                         ####  " << title << "  ####\n\n";
	setDefaultColor();
}

inline void successMsg(const string& msg)
{
	setColor(10); /* bright green */
	cout << "\n\n                         [OK]  " << msg << "\n\n";
	setDefaultColor();
}

inline void errorMsg(const string& msg)
{
	setColor(12); /* bright red */
	cout << "\n\n                         [ERROR]  " << msg << "\n\n";
	setDefaultColor();
}

inline void infoMsg(const string& msg)
{
	setColor(11); /* bright cyan */
	cout << "\n                         [INFO]  " << msg << "\n";
	setDefaultColor();
}

inline int readIntInRange(const string& prompt, int lo, int hi)
{
	int value;
	setColor(15);
	cout << prompt;
	setDefaultColor();
	while (!(cin >> value) || value < lo || value > hi)
	{
		setColor(12);
		cout << "                         Invalid input. Enter an integer in range [" << lo << " - " << hi << "]:   ";
		cin.clear();
		cin.ignore(MAXDWORD, '\n');
		setDefaultColor();
	}
	cin.ignore((numeric_limits<streamsize>::max)(), '\n');
	return value;
}

inline string readLine(const string& prompt)
{
	string s;
	setColor(15);
	cout << prompt;
	setDefaultColor();
	getline(cin, s);
	return s;
}

inline string readToken(const string& prompt)
{
	string s;
	setColor(15);
	cout << prompt;
	setDefaultColor();
	cin >> s;
	cin.ignore((numeric_limits<streamsize>::max)(), '\n');
	return s;
}

inline string getPasswordMasked()
{
	if (!_isatty(_fileno(stdin)))
	{
		string password;
		getline(cin, password);
		return password;
	}

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
	enableDarkTheme();
	setFontBold(24);
	clearScreen();
	setColor(11);
	cout << "\n\n\n";
	cout << "               ================================================================================================\n";
	cout << "                                    WELCOME TO FAST SHOPPING PORTAL\n";
	cout << "               ================================================================================================\n";
	setColor(14);
	cout << "\n";
	cout << "                                   Author: Mohammad Rohaan  |  22I-2327  |  Sec-Z\n";
	setColor(7);
	cout << "                                   OOP Console Shopping Portal - Admin | Vendor | Customer\n\n\n";
	setDefaultColor();
}

inline int Select_Role()
{
	setColor(14);
	cout << "\n\n                                                         What Is Your Login Type?\n\n";
	setColor(15);
	cout << "                                                               1. Admin\n\n";
	cout << "                                                               2. Vendor\n\n";
	cout << "                                                               3. Customer\n\n";
	cout << "                                                               4. Exit / Close Window\n\n";
	setDefaultColor();
	return readIntInRange("                                      Enter Your Choice here                   ", 1, 4);
}

#endif // !OTHER_FUN_H
