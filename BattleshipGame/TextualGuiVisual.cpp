#include "TextualGuiVisual.h"
#include "BattleBoard.h"
#include <Windows.h>
#include <iostream>
#include <stdlib.h>

using std::cout;
using std::endl;

namespace battleship
{
	/** Points the cursor at (row,col) in the console output */
	void gotoxy(int row, int col)
	{
		COORD coord;
		coord.X = col;
		coord.Y = row;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	}

	/** Sets the text color for the console to the given color enum */
	void setTextColor(ConsoleColor color)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, (int)color);
	}

	/** Clears the console screen */
	void clearScreen()
	{
		system("cls");
	}

	/** Shows / Hides the console cursor according to isVisible parameter */
	void setConsoleCursor(bool isVisible)
	{
		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO info;
		info.dwSize = 100;
		info.bVisible = isVisible;
		SetConsoleCursorInfo(consoleHandle, &info);
	}

	TextualGuiVisual::TextualGuiVisual(int delayMs):
		IGameVisual(),
		_delayMs(delayMs)
	{
	}

	ConsoleColor TextualGuiVisual::getColorForSquare(shared_ptr<BattleBoard> board,
													 int row, int col)
	{
		auto matrix = board->getBoardMatrix();
		char currSquare = matrix[row][col];
		ConsoleColor color;

		if (board->whichPlayerOwnsSquare(row, col) == PlayerEnum::A)
		{
			color = PLAYER_A_COLOR_MAPPING.at((BoardSquare)toupper(currSquare));
		}
		else if (board->whichPlayerOwnsSquare(row, col) == PlayerEnum::B)
		{
			color = PLAYER_B_COLOR_MAPPING.at((BoardSquare)toupper(currSquare));
		}
		else
		{
			color = PLAYER_A_COLOR_MAPPING.at(BoardSquare::Empty);
		}

		return color;
	}

	void TextualGuiVisual::printBoard(shared_ptr<BattleBoard> board)
	{
		auto matrix = board->getBoardMatrix();

		for (int row = 0; row < BOARD_SIZE; row++)
		{
			for (int col = 0; col < BOARD_SIZE; col++)
			{
				char currSquare = matrix[row][col];
				ConsoleColor color = getColorForSquare(board, row, col);

				gotoxy(row, col);
				setTextColor(color);
				cout << currSquare;
			}

			cout << endl;
		}
	}

	void TextualGuiVisual::visualizeBeginGame(shared_ptr<BattleBoard> board)
	{
		setConsoleCursor(false);
		clearScreen();
		printBoard(board);
	}

	void TextualGuiVisual::eraseMoveDescription()
	{
		int titleRow = BOARD_SIZE / 3;
		int titleCol = BOARD_SIZE + 4;
		gotoxy(titleRow, titleCol);
		setTextColor(ConsoleColor::WHITE);
		string descriptionPlaceholder = "Player A attacks at (10, 10)";

		for (int i = 0; i < descriptionPlaceholder.length(); i++)
			cout << " ";

		gotoxy(titleRow + 1, titleCol);
		descriptionPlaceholder = "Results: Miss      ";

		for (int j = 0; j < descriptionPlaceholder.length(); j++)
			cout << " ";
	}

	void TextualGuiVisual::printLastAttackResultsDesc(shared_ptr<BattleBoard> board,
													  int row, int col, AttackResult attackResults)
	{
		gotoxy(TITLE_ROW + 1, TITLE_COL);
		setTextColor(ConsoleColor::WHITE);
		cout << "Results: ";

		if (attackResults == AttackResult::Miss)
		{
			setTextColor(ConsoleColor::LIGHT_GREEN);
			cout << "Miss  ";
		}
		else if (attackResults == AttackResult::Hit)
		{
			setTextColor(ConsoleColor::LIGHT_YELLOW);
			cout << "Hit   ";
		}
		else
		{ // Sink
			setTextColor(ConsoleColor::LIGHT_RED);
			cout << "Sink   ";
		}

		setTextColor(ConsoleColor::WHITE);
	}

	void TextualGuiVisual::printLastMoveDescription(shared_ptr<BattleBoard> board, int attackingPlayerNumber,
													int row, int col)
	{
		
		gotoxy(TITLE_ROW, TITLE_COL);
		setTextColor(ConsoleColor::WHITE);
		cout << "Player ";

		if (attackingPlayerNumber == 0)
		{ // A = 0
			setTextColor(ConsoleColor::LIGHT_PURPLE);
			cout << "A";
		}
		else
		{ // B = 1
			setTextColor(ConsoleColor::LIGHT_TURQUOISE);
			cout << "B";
		}

		setTextColor(ConsoleColor::WHITE);
		cout << " attacks at (" << (row + 1) << "," << (col + 1) << ")       ";
	}

	void TextualGuiVisual::visualizeAttackResults(shared_ptr<BattleBoard> board, int attackingPlayerNumber,
												  int row, int col, AttackResult attackResults,
												  shared_ptr<const GamePiece> attackedGamePiece)
	{
		eraseMoveDescription();
		printLastMoveDescription(board, attackingPlayerNumber, row, col);
		auto matrix = board->getBoardMatrix();
		char endOfAnimationChar = matrix[row][col];

		ConsoleColor color = PLAYER_A_COLOR_MAPPING.at(BoardSquare::Bombared);
		gotoxy(row, col);
		setTextColor(color);
		cout << (char)BoardSquare::Bombared;
		Sleep(_delayMs);

		if (attackResults != AttackResult::Sink)
		{
			color = getColorForSquare(board, row, col);
			gotoxy(row, col);
			setTextColor(color);
			cout << endOfAnimationChar;
			printLastAttackResultsDesc(board, row, col, attackResults);
		}
		else
		{ // Sink
			int currRow = attackedGamePiece->_firstRow;
			int currCol = attackedGamePiece->_firstCol;
			int rowOffset = (attackedGamePiece->_orient == Orientation::VERTICAL) ? 1 : 0;
			int colOffset = (attackedGamePiece->_orient == Orientation::HORIZONTAL) ? 1 : 0;

			for (int i = 0; i < attackedGamePiece->_shipType->_size; i++)
			{
				gotoxy(currRow, currCol);
				setTextColor(ConsoleColor::LIGHT_YELLOW_BLUE_BG);
				cout << (char)BoardSquare::Sinking;
				currRow += rowOffset;
				currCol += colOffset;
			}

			printLastAttackResultsDesc(board, row, col, attackResults);

			Sleep(_delayMs);

			currRow = attackedGamePiece->_firstRow;
			currCol = attackedGamePiece->_firstCol;

			for (int j = 0; j < attackedGamePiece->_shipType->_size; j++)
			{
				color = getColorForSquare(board, currRow, currCol);
				gotoxy(currRow, currCol);
				setTextColor(color);
				cout << (char)BoardSquare::Empty;
				currRow += rowOffset;
				currCol += colOffset;
			}
		}


		gotoxy(BOARD_SIZE + 1, 0); // Move cursor in case the rest of the app wants to print something
		Sleep(_delayMs);
	}

	void TextualGuiVisual::visualizeEndGame(shared_ptr<BattleBoard> board, int playerAPoints, int playerBPoints)
	{
		gotoxy(BOARD_SIZE + 1, 0);
		setTextColor(ConsoleColor::WHITE);
		setConsoleCursor(true);
		IGameVisual::visualizeEndGame(board, playerAPoints, playerBPoints);
	}
}