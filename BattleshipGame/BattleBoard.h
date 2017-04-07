#pragma once

#include <string>
using std::string;

const size_t BOARD_SIZE = 10;

enum class ShipType {
	RubberBoat, RocketShip, Submarine, Battleship
};

class BattleBoard
{
public:
	string playerName;
	char matrix[BOARD_SIZE][BOARD_SIZE];

	BattleBoard();
	~BattleBoard();
};