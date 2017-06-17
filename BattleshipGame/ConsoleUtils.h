#pragma once

#include <windows.h>

namespace battleship
{
	/** Util for controling console output in windows */
	class ConsoleUtils
	{
	public:
		virtual ~ConsoleUtils() = delete;	// Disallow allocation of this helper object
											// (more precisely - deallocation)

		// Get (x,y) of Console cursor
		static COORD getConsoleCursorPosition();

		// Set Console cursor to given position
		static void gotoxy(int row, int col);

		// Show / hide Console's cursor
		static void setConsoleCursor(bool isVisible);

		// Activate shutdown callback, to restore console cursor if program quits unexpectedly
		static void registerCloseupHandler();

	private:
		ConsoleUtils() = default; // Hide constructor
		static BOOL WINAPI closeupHandler(DWORD signal);
	};
}
