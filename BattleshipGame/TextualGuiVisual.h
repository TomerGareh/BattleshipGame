#pragma once

#include "IGameVisual.h"
#include <memory>
#include <map>

using std::shared_ptr;
using std::map;

namespace battleship
{
	enum class ConsoleColor : int
	{
		DARK_BLUE = 1,
		DARK_GREEN = 2,
		DARK_RED = 4,
		DARK_PURPLE = 5,
		DARK_YELLOW = 6,
		WHITE = 7,
		SILVER = 8,
		LIGHT_BLUE = 9,
		LIGHT_GREEN = 10,
		LIGHT_RED = 12,
		LIGHT_PURPLE = 13,
		LIGHT_YELLOW = 14,

		BLUE_BLUE_BG = 17,
		DARK_GREEN_BLUE_BG = 18,
		DARK_RED_BLUE_BG = 20,
		DARK_PURPLE_BLUE_BG = 21,
		DARK_YELLOW_BLUE_BG = 22,
		WHITE_BLUE_BG = 23,
		SILVER_BLUE_BG = 24,
		LIGHT_BLUE_BLUE_BG = 25,
		LIGHT_GREEN_BLUE_BG = 26,
		LIGHT_RED_BLUE_BG = 28,
		LIGHT_PURPLE_BLUE_BG = 29,
		LIGHT_YELLOW_BLUE_BG = 30
	};

	class TextualGuiVisual : public IGameVisual
	{
	public:
		TextualGuiVisual(int delayMs);
		virtual ~TextualGuiVisual() = default;

		virtual void visualizeBeginGame(shared_ptr<BattleBoard> board) override;

		virtual void visualizeAttackResults(shared_ptr<BattleBoard> board,
											int row, int col, AttackResult attackResults) override;

	private:
		const map<BoardSquare, ConsoleColor> PLAYER_A_COLOR_MAPPING = 
		{
			{ BoardSquare::Empty, ConsoleColor::BLUE_BLUE_BG },
			{ BoardSquare::Hit, ConsoleColor::WHITE_BLUE_BG },
			{ BoardSquare::Bombared, ConsoleColor::LIGHT_RED },
			{ BoardSquare::RubberBoat, ConsoleColor::LIGHT_PURPLE_BLUE_BG },
			{ BoardSquare::RocketShip, ConsoleColor::LIGHT_GREEN_BLUE_BG },
			{ BoardSquare::Submarine, ConsoleColor::LIGHT_YELLOW_BLUE_BG },
			{ BoardSquare::Battleship, ConsoleColor::LIGHT_RED_BLUE_BG },
		};

		const map<BoardSquare, ConsoleColor> PLAYER_B_COLOR_MAPPING =
		{
			{ BoardSquare::Empty, ConsoleColor::BLUE_BLUE_BG },
			{ BoardSquare::Hit, ConsoleColor::SILVER_BLUE_BG },
			{ BoardSquare::Bombared, ConsoleColor::LIGHT_YELLOW },
			{ BoardSquare::RubberBoat, ConsoleColor::DARK_PURPLE_BLUE_BG },
			{ BoardSquare::RocketShip, ConsoleColor::DARK_GREEN_BLUE_BG },
			{ BoardSquare::Submarine, ConsoleColor::DARK_YELLOW_BLUE_BG },
			{ BoardSquare::Battleship, ConsoleColor::DARK_RED_BLUE_BG },
		};

		int _delayMs;

		void printBoard(shared_ptr<BattleBoard> board);
		ConsoleColor getColorForSquare(shared_ptr<BattleBoard> board, int row, int col);
	};
}