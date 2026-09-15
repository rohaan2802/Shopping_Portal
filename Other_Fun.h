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

/** Extra buffer rows so the vertical scrollbar works. */
constexpr SHORT CONSOLE_SCROLLBACK_ROWS = 500;

inline void getVisibleConsoleSize(SHORT& winW, SHORT& winH)
{
	winW = 80;
	winH = 25;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(console(), &csbi))
	{
		winW = static_cast<SHORT>(csbi.srWindow.Right - csbi.srWindow.Left + 1);
		winH = static_cast<SHORT>(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
	}
	if (winW < 40) winW = 80;
	if (winH < 8) winH = 25;
}

inline void applyBufferLayout(HANDLE hOut, SHORT winW, SHORT winH, WORD attrs)
{
	if (winW < 40) winW = 80;
	if (winH < 8) winH = 25;

	SHORT bufH = static_cast<SHORT>(winH + CONSOLE_SCROLLBACK_ROWS);
	if (bufH > 3000) bufH = 3000;

	COORD bufSize = { winW, bufH };
	SMALL_RECT winRect = { 0, 0, static_cast<SHORT>(winW - 1), static_cast<SHORT>(winH - 1) };

	/* Buffer first (must be >= window), then visible window — keeps scrollbar. */
	SetConsoleScreenBufferSize(hOut, bufSize);
	SetConsoleWindowInfo(hOut, TRUE, &winRect);
	SetConsoleTextAttribute(hOut, attrs);
	COORD origin = { 0, 0 };
	SetConsoleCursorPosition(hOut, origin);
}

/** Startup only: ensure current buffer has scrollback + matched width. */
inline void syncConsoleBufferToWindow()
{
	SHORT winW = 80, winH = 25;
	getVisibleConsoleSize(winW, winH);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	WORD attrs = 0x0F;
	if (GetConsoleScreenBufferInfo(console(), &csbi))
		attrs = csbi.wAttributes;
	applyBufferLayout(console(), winW, winH, attrs);
}

inline void rebindStdStreamsToConsole()
{
	FILE* fp = nullptr;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
	freopen_s(&fp, "CONIN$", "r", stdin);
	setvbuf(stdout, nullptr, _IONBF, 0);
	ios::sync_with_stdio(true);
	cout.clear();
	cerr.clear();
	cin.clear();
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
	syncConsoleBufferToWindow();
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
	/* Permanent anti-ghost: swap to a fresh console screen buffer.
	 * Fill/Scroll/cls leave conhost paint artifacts (especially after errors + large font).
	 * New buffer = clean slate; scrollback rows keep the scrollbar. */
	cout.flush();
	cerr.flush();

	HANDLE hOld = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD attrs = 0x0F;
	SHORT winW = 80, winH = 25;
	CONSOLE_FONT_INFOEX font{};
	font.cbSize = sizeof(font);
	bool haveFont = false;

	CONSOLE_SCREEN_BUFFER_INFO csbi{};
	if (hOld != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hOld, &csbi))
	{
		attrs = csbi.wAttributes;
		winW = static_cast<SHORT>(csbi.srWindow.Right - csbi.srWindow.Left + 1);
		winH = static_cast<SHORT>(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
		haveFont = (GetCurrentConsoleFontEx(hOld, FALSE, &font) != 0);
	}
	if (winW < 40) winW = 80;
	if (winH < 8) winH = 25;

	SECURITY_ATTRIBUTES sa{};
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = nullptr;
	sa.bInheritHandle = TRUE;

	HANDLE hNew = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&sa,
		CONSOLE_TEXTMODE_BUFFER,
		nullptr);

	if (hNew == INVALID_HANDLE_VALUE)
	{
		system("cls");
		setDefaultColor();
		return;
	}

	applyBufferLayout(hNew, winW, winH, attrs);
	if (haveFont)
		SetCurrentConsoleFontEx(hNew, FALSE, &font);

	if (!SetConsoleActiveScreenBuffer(hNew))
	{
		CloseHandle(hNew);
		system("cls");
		setDefaultColor();
		return;
	}

	SetStdHandle(STD_OUTPUT_HANDLE, hNew);
	rebindStdStreamsToConsole();

	/* Close only buffers we previously created (never drop the first OS buffer handle). */
	static HANDLE s_ownedBuffer = INVALID_HANDLE_VALUE;
	if (s_ownedBuffer != INVALID_HANDLE_VALUE && s_ownedBuffer != hNew)
		CloseHandle(s_ownedBuffer);
	s_ownedBuffer = hNew;

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

/** Username / text cancel: "0" or "back" (case-insensitive). */
inline bool isCancelToken(const string& s)
{
	string t = trimCopy(s);
	if (t.empty()) return false;
	if (t == "0") return true;
	for (char& c : t)
		c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
	return t == "back" || t == "b" || t == "cancel";
}

/** Read first field; if user cancels, shows message and returns true. */
inline bool readTokenOrCancel(const string& prompt, string& out)
{
	out = readToken(prompt);
	if (isCancelToken(out))
	{
		infoMsg("Going back...");
		out.clear();
		return true;
	}
	return false;
}

/** Read a line; empty / 0 / back cancels. */
inline bool readLineOrCancel(const string& prompt, string& out)
{
	out = trimCopy(readLine(prompt));
	if (out.empty() || isCancelToken(out))
	{
		infoMsg("Going back...");
		out.clear();
		return true;
	}
	return false;
}

/** After a failed form: 1 = retry, 0 = back to previous menu. */
inline bool askRetryOrBack()
{
	cout << "\n";
	int ch = readIntInRange("1 = Try again   |   0 = Go Back:   ", 0, 1);
	return ch == 1;
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

/** Parse one order line.
 *  New:  id|user|total|date|Name:qty;Name:qty
 *  Old:  id|user|total|Name xqty; Name xqty @ date
 */
inline bool parseOrderLine(const string& line,
	string& id, string& user, string& total, string& date, string& itemsBlob)
{
	id.clear(); user.clear(); total.clear(); date.clear(); itemsBlob.clear();
	stringstream ss(line);
	string f0, f1, f2, rest;
	if (!getline(ss, f0, '|')) return false;
	if (!getline(ss, f1, '|')) return false;
	if (!getline(ss, f2, '|')) return false;
	getline(ss, rest);
	id = trimCopy(f0);
	user = trimCopy(f1);
	total = trimCopy(f2);
	rest = trimCopy(rest);
	if (id.empty()) return false;

	/* New format: rest starts with date then | then items */
	size_t bar = rest.find('|');
	if (bar != string::npos)
	{
		date = trimCopy(rest.substr(0, bar));
		itemsBlob = trimCopy(rest.substr(bar + 1));
		return true;
	}

	/* Old format: "... @ YYYY-MM-DD HH:MM" */
	size_t at = rest.rfind(" @ ");
	if (at != string::npos)
	{
		itemsBlob = trimCopy(rest.substr(0, at));
		date = trimCopy(rest.substr(at + 3));
	}
	else
		itemsBlob = rest;
	return true;
}

/** Split "Apples:2;Bread:1" or "Apples x2; Bread x1" into display lines. */
inline void printOrderItemsList(const string& itemsBlob)
{
	if (trimCopy(itemsBlob).empty())
	{
		contentPrint("  (no items listed)");
		return;
	}

	stringstream items(itemsBlob);
	string piece;
	int n = 0;
	while (getline(items, piece, ';'))
	{
		piece = trimCopy(piece);
		if (piece.empty()) continue;
		n++;

		string name;
		string qtyStr = "1";
		size_t colon = piece.find(':');
		size_t xPos = piece.rfind(" x");
		if (colon != string::npos)
		{
			name = trimCopy(piece.substr(0, colon));
			qtyStr = trimCopy(piece.substr(colon + 1));
		}
		else if (xPos != string::npos)
		{
			name = trimCopy(piece.substr(0, xPos));
			qtyStr = trimCopy(piece.substr(xPos + 2));
		}
		else
			name = piece;

		ostringstream row;
		row << "  " << paddedIndex(n, 2) << ")  "
			<< fitField(name, 28) << "  Qty: " << qtyStr;
		contentPrint(row.str());
	}
	if (n == 0)
		contentPrint("  (no items listed)");
}

/** Pretty-print one order (Admin stats / Customer history). */
inline void printOrderBlock(const string& id, const string& user,
	const string& total, const string& date, const string& itemsBlob)
{
	setColor(11);
	contentPrint("Order #" + id);
	setDefaultColor();
	contentPrint("Customer : " + user);
	if (!date.empty())
		contentPrint("Date     : " + date);
	contentPrint("Total    : Rs. " + total);
	contentPrint("Items:");
	printOrderItemsList(itemsBlob);
	cout << "\n";
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
	centerPrint("WELCOME TO FAST SHOPPING PORTAL");
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
	contentPrint("What Is Your Login Type?");
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
