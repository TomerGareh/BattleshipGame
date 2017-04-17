#pragma once

#include "IGameVisual.h"
#include <memory>
#include <map>

using std::shared_ptr;
using std::map;

namespace battleship
{
	/** Supported colors by the console */
	enum class ConsoleColor : int
	{
		DARK_BLUE = 1,
		DARK_GREEN = 2,
		DARK_TURQUOISE = 3,
		DARK_RED = 4,
		DARK_PURPLE = 5,
		DARK_YELLOW = 6,
		SILVER = 8,
		LIGHT_BLUE = 9,
		LIGHT_GREEN = 10,
		LIGHT_TURQUOISE = 11,
		LIGHT_RED = 12,
		LIGHT_PURPLE = 13,
		LIGHT_YELLOW = 14,
		WHITE = 15,

		BLUE_BLUE_BG = 17,
		DARK_GREEN_BLUE_BG = 18,
		DARK_TURQUOISE_BLUE_BG = 19,
		DARK_RED_BLUE_BG = 20,
		DARK_PURPLE_BLUE_BG = 21,
		DARK_YELLOW_BLUE_BG = 22,
		SILVER_BLUE_BG = 24,
		LIGHT_BLUE_BLUE_BG = 25,
		LIGHT_GREEN_BLUE_BG = 26,
		LIGHT_TURQUOISE_BLUE_BG = 27,
		LIGHT_RED_BLUE_BG = 28,
		LIGHT_PURPLE_BLUE_BG = 29,
		LIGHT_YELLOW_BLUE_BG = 30,
		WHITE_BLUE_BG = 31
	};

	/** A visualizer that uses textual animations and console colors to describe the game session */
	class TextualGuiVisual : public IGameVisual
	{
	public:
		/** Creates a new visualizer instance, with delay between animations in milliseconds */
		TextualGuiVisual(int delayMs);
		virtual ~TextualGuiVisual() = default;

		virtual void visualizeBeginGame(shared_ptr<BattleBoard> board) override;

		virtual void visualizeAttackResults(shared_ptr<BattleBoard> board, int attackingPlayerNumber,
											int row, int col, AttackResult attackResults,
											shared_ptr<const GamePiece> attackedGamePiece) override;

		virtual void visualizeEndGame(shared_ptr<BattleBoard> board, int playerAWins, int playerBWins) override;

		/** Shows / Hides the console cursor according to isVisible parameter */
		static void setConsoleCursor(bool isVisible);

	private:
		/** Position of title on console window */
		const int TITLE_ROW = BOARD_SIZE / 3;
		const int TITLE_COL = BOARD_SIZE + 4;

		/** Colors used by Player A pieces */
		const map<BoardSquare, ConsoleColor> PLAYER_A_COLOR_MAPPING = 
		{
			{ BoardSquare::Empty, ConsoleColor::BLUE_BLUE_BG },
			{ BoardSquare::Hit, ConsoleColor::WHITE_BLUE_BG },
			{ BoardSquare::Bombared, ConsoleColor::WHITE },
			{ BoardSquare::RubberBoat, ConsoleColor::LIGHT_TURQUOISE_BLUE_BG },
			{ BoardSquare::RocketShip, ConsoleColor::LIGHT_GREEN_BLUE_BG },
			{ BoardSquare::Submarine, ConsoleColor::LIGHT_YELLOW_BLUE_BG },
			{ BoardSquare::Battleship, ConsoleColor::LIGHT_RED_BLUE_BG },
		};

		/** Colors used by Player B pieces */
		const map<BoardSquare, ConsoleColor> PLAYER_B_COLOR_MAPPING =
		{
			{ BoardSquare::Empty, ConsoleColor::BLUE_BLUE_BG },
			{ BoardSquare::Hit, ConsoleColor::WHITE_BLUE_BG },
			{ BoardSquare::Bombared, ConsoleColor::WHITE },
			{ BoardSquare::RubberBoat, ConsoleColor::DARK_TURQUOISE_BLUE_BG },
			{ BoardSquare::RocketShip, ConsoleColor::DARK_GREEN_BLUE_BG },
			{ BoardSquare::Submarine, ConsoleColor::DARK_YELLOW_BLUE_BG },
			{ BoardSquare::Battleship, ConsoleColor::DARK_RED_BLUE_BG },
		};

		/** Delay between animations, in milliseconds */
		int _delayMs;

		/** Prints the given board status to the console */
		void printBoard(shared_ptr<BattleBoard> board);

		/** Returns which color should be used for the game-square in the given indices (according to
		 *  which player owns the square, which piece is it, is it blank, etc).
		 */
		ConsoleColor getColorForSquare(shared_ptr<BattleBoard> board, int row, int col);

		/** Prints the details of the last attack by a player */
		void printLastMoveDescription(shared_ptr<BattleBoard> board, int attackingPlayerNumber,
									  int row, int col);

		/** Prints the details of the last attack results by a player */
		void printLastAttackResultsDesc(shared_ptr<BattleBoard> board,
										int row, int col, AttackResult attackResults);

		/** Erase the previous move and attack result descriptions */
		void eraseMoveDescription();

		/** Points the cursor at (row,col) in the console output */
		static void gotoxy(int row, int col);

		/** Sets the text color for the console to the given color enum */
		static void setTextColor(ConsoleColor color);

		/** Clears the console screen */
		static void clearScreen();
	};
}