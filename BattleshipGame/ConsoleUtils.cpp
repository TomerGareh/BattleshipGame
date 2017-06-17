#include "ConsoleUtils.h"

namespace battleship
{
	COORD ConsoleUtils::getConsoleCursorPosition()
	{
		HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO cbsi;
		if (GetConsoleScreenBufferInfo(hConsoleOutput, &cbsi))
		{
			return cbsi.dwCursorPosition;
		}
		else
		{
			// The function failed. Use 0,0 as fallback
			COORD invalid = { 0, 0 };
			return invalid;
		}
	}

	void ConsoleUtils::gotoxy(int row, int col)
	{
		COORD coord;
		coord.X = col;
		coord.Y = row;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	}

	void ConsoleUtils::setConsoleCursor(bool isVisible)
	{
		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO info;
		info.dwSize = 100;
		info.bVisible = isVisible;
		SetConsoleCursorInfo(consoleHandle, &info);
	}

	BOOL WINAPI ConsoleUtils::closeupHandler(DWORD signal)
	{
		// Make sure cursor is restored even if program is quit unexpectedly
		if (signal == CTRL_C_EVENT)
			setConsoleCursor(true);

		return FALSE;
	}

	void ConsoleUtils::registerCloseupHandler()
	{
		SetConsoleCtrlHandler(closeupHandler, TRUE);
	}
}