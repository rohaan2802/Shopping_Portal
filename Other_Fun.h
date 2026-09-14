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
#include <fstream>
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

/** Standard content column (~62 chars). Capped usable width avoids maximized-console
 *  "everything shoved to the right" and next-line wrap. */
constexpr int CONTENT_WIDTH = 62;
constexpr int USABLE_WIDTH_MAX = 88;
constexpr int USABLE_WIDTH_MIN = 70;

inline HANDLE console()
{
	return GetStdHandle(STD_OUTPUT_HANDLE);
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

inline int getConsoleWidth()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(console(), &csbi))
	{
		int w = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		if (w >= 40)
			return w;
	}
	return 80;
}

/** Cap width so ultra-wide / bad buffer reports cannot push UI to the right. */
inline int getUsableWidth()
{
	int w = getConsoleWidth();
	if (w > USABLE_WIDTH_MAX)
		w = USABLE_WIDTH_MAX;
	if (w < USABLE_WIDTH_MIN)
		w = USABLE_WIDTH_MIN;
	return w;
}

inline int contentLeftPad()
{
	int pad = (getUsableWidth() - CONTENT_WIDTH) / 2;
	if (pad < 2)
		pad = 2;
	if (pad > 14)
		pad = 14;
	return pad;
}

inline int maxLineChars()
{
	return CONTENT_WIDTH;
}

inline string truncateFit(const string& s, int maxLen)
{
	if (maxLen < 4)
		maxLen = 4;
	if (static_cast<int>(s.size()) <= maxLen)
		return s;
	return s.substr(0, static_cast<size_t>(maxLen - 3)) + "...";
}

inline string fitField(const string& s, int width, bool leftAlign = true)
{
	string t = truncateFit(s, width);
	ostringstream oss;
	if (leftAlign)
		oss << left << setfill(' ') << setw(width) << t;
	else
		oss << right << setfill(' ') << setw(width) << t;
	return oss.str();
}

/** Center inside the content column (standard look — not full-screen ultra-wide). */
inline void centerPrint(const string& text, bool newline = true)
{
	string t = truncateFit(text, CONTENT_WIDTH);
	int inner = (CONTENT_WIDTH - static_cast<int>(t.size())) / 2;
	if (inner < 0)
		inner = 0;
	cout << string(static_cast<size_t>(contentLeftPad() + inner), ' ') << t;
	if (newline)
		cout << "\n";
}

inline void centerPrintColored(const string& text, int color, bool newline = true)
{
	setColor(color);
	centerPrint(text, newline);
	setDefaultColor();
}

/** Left-aligned inside the centered CONTENT_WIDTH column. */
inline void contentPrint(const string& text, bool newline = true)
{
	string t = truncateFit(text, CONTENT_WIDTH);
	cout << string(static_cast<size_t>(contentLeftPad()), ' ') << t;
	if (newline)
		cout << "\n";
}

inline void contentPrintColored(const string& text, int color, bool newline = true)
{
	setColor(color);
	contentPrint(text, newline);
	setDefaultColor();
}

inline void printPaddedIndex(ostream& out, int n, int width = 2)
{
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

inline void printNumberedLine(ostream& out, int n, const string& text, int width = 2)
{
	ostringstream line;
	line << paddedIndex(n, width) << ")  " << text;
	string s = truncateFit(line.str(), CONTENT_WIDTH);
	out << string(static_cast<size_t>(contentLeftPad()), ' ') << s << "\n";
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
		return idx > 0;
	}
	catch (...)
	{
		return false;
	}
}

inline bool fileExists(const string& path)
{
	ifstream f(path);
	return f.good();
}

inline bool isBlank(const string& s)
{
	return trimCopy(s).empty();
}

inline void clearScreen()
{
	system("cls");
	setDefaultColor();
}

inline void pauseEnter()
{
	setColor(8);
	cout << "\n\n";
	centerPrint("Press Enter to continue...", false);
	setDefaultColor();
	cin.clear();
	string dummy;
	getline(cin, dummy);
}

inline void centerBanner(const string& title, int color = 11)
{
	string bar(static_cast<size_t>(CONTENT_WIDTH), '=');
	setColor(color);
	cout << "\n\n";
	contentPrint(bar);
	contentPrint(truncateFit(title, CONTENT_WIDTH));
	contentPrint(bar);
	cout << "\n\n";
	setDefaultColor();
}

inline void banner(const string& title, int color = 11)
{
	centerBanner(title, color);
}

inline void sectionTitle(const string& title, int color = 14)
{
	setColor(color);
	cout << "\n\n";
	centerPrint("####  " + truncateFit(title, CONTENT_WIDTH - 10) + "  ####");
	cout << "\n";
	setDefaultColor();
}

/** Word-wrap full message into CONTENT_WIDTH lines (no "..." truncation). */
inline void contentPrintWrapped(const string& text)
{
	string remaining = text;
	while (!remaining.empty())
	{
		if (static_cast<int>(remaining.size()) <= CONTENT_WIDTH)
		{
			contentPrint(remaining);
			break;
		}
		int cut = CONTENT_WIDTH;
		/* Prefer breaking at a space so words stay readable */
		int breakAt = cut;
		for (int i = cut; i >= CONTENT_WIDTH / 2; --i)
		{
			if (remaining[static_cast<size_t>(i)] == ' ')
			{
				breakAt = i;
				break;
			}
		}
		contentPrint(remaining.substr(0, static_cast<size_t>(breakAt)));
		remaining = remaining.substr(static_cast<size_t>(breakAt));
		while (!remaining.empty() && remaining.front() == ' ')
			remaining.erase(remaining.begin());
	}
}

inline void successMsg(const string& msg)
{
	setColor(10); /* bright green */
	cout << "\n\n";
	contentPrintWrapped("[OK]  " + msg);
	cout << "\n";
	setDefaultColor();
}

inline void errorMsg(const string& msg)
{
	setColor(12); /* bright red */
	cout << "\n\n";
	contentPrintWrapped("[ERROR]  " + msg);
	cout << "\n";
	setDefaultColor();
}

inline void infoMsg(const string& msg)
{
	setColor(11); /* bright cyan */
	cout << "\n";
	contentPrintWrapped("[INFO]  " + msg);
	setDefaultColor();
}

/** Emit a prompt inside the content block (leading newlines preserved, indent stripped). */
inline void emitPrompt(const string& prompt)
{
	string p = prompt;
	while (!p.empty() && p.front() == '\n')
	{
		cout << '\n';
		p.erase(p.begin());
	}
	while (!p.empty() && (p.front() == ' ' || p.front() == '\t'))
		p.erase(p.begin());
	setColor(15);
	contentPrint(p, false);
	setDefaultColor();
}

inline int readIntInRange(const string& prompt, int lo, int hi)
{
	int value;
	emitPrompt(prompt);
	while (!(cin >> value) || value < lo || value > hi)
	{
		setColor(12);
		cout << "\n";
		ostringstream oss;
		oss << "Invalid input. Enter an integer in range [" << lo << " - " << hi << "]:   ";
		contentPrint(oss.str(), false);
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
	emitPrompt(prompt);
	getline(cin, s);
	return s;
}

inline string readToken(const string& prompt)
{
	string s;
	emitPrompt(prompt);
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
	string bar(static_cast<size_t>(CONTENT_WIDTH), '=');
	setColor(11);
	cout << "\n\n\n";
	contentPrint(bar);
	contentPrint("WELCOME TO FAST SHOPPING PORTAL");
	contentPrint(bar);
	setColor(14);
	cout << "\n";
	contentPrint("Author: Mohammad Rohaan  |  22I-2327  |  Sec-Z");
	setColor(7);
	contentPrint("OOP Console Shopping Portal - Admin | Vendor | Customer");
	cout << "\n\n";
	setDefaultColor();
}

inline int Select_Role()
{
	setColor(14);
	cout << "\n\n";
	centerPrint("What Is Your Login Type?");
	cout << "\n";
	setColor(15);
	contentPrint("1. Admin");
	cout << "\n";
	contentPrint("2. Vendor");
	cout << "\n";
	contentPrint("3. Customer");
	cout << "\n";
	contentPrint("4. Exit / Close Window");
	cout << "\n";
	setDefaultColor();
	return readIntInRange("Enter Your Choice here:   ", 1, 4);
}

#endif // !OTHER_FUN_H
