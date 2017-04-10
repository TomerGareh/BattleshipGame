#include "TextualGuiVisual.h"
#include <Windows.h>
#include <iostream>
#include "BattleBoard.h"

using std::cout;
using std::endl;

namespace battleship
{
	void gotoxy(int x, int y)
	{
		COORD coord;
		coord.X = x;
		coord.Y = y;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	}

	void setTextColor(ConsoleColor color)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, (int)color);
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
			color = PLAYER_A_COLOR_MAPPING.at((BoardSquare)currSquare);
		}
		else if (board->whichPlayerOwnsSquare(row, col) == PlayerEnum::B)
		{
			color = PLAYER_B_COLOR_MAPPING.at((BoardSquare)currSquare);
		}
		else
		{
			color = PLAYER_A_COLOR_MAPPING.at(BoardSquare::Empty);
		}
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
		printBoard(board);
	}

	void TextualGuiVisual::visualizeAttackResults(shared_ptr<BattleBoard> board,
												  int row, int col, AttackResult attackResults)
	{
		auto matrix = board->getBoardMatrix();
		char endOfAnimationChar = matrix[row][col];

		if ((attackResults == AttackResult::Hit) || (attackResults == AttackResult::Sink))
		{
			ConsoleColor color = (board->whichPlayerOwnsSquare(row, col) == PlayerEnum::A) ?
								  PLAYER_A_COLOR_MAPPING.at(BoardSquare::Bombared) : 
								  PLAYER_B_COLOR_MAPPING.at(BoardSquare::Bombared);
			gotoxy(row, col);
			setTextColor(color);
			cout << (char)BoardSquare::Bombared;
			Sleep(_delayMs);
		}

		ConsoleColor color = getColorForSquare(board, row, col);
		gotoxy(row, col);
		setTextColor(color);
		cout << endOfAnimationChar;
	}
}