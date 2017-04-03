#pragma once

#include <string>

const size_t BOARD_SIZE = 10;

class BattleBoard
{
public:
	std::string playerName;
	char matrix[BOARD_SIZE][BOARD_SIZE];

	BattleBoard();
	~BattleBoard();
};