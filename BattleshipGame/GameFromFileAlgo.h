#pragma once

#include "IBattleshipGameAlgo.h"
#include "BattleBoard.h"

class GameFromFileAlgo : IBattleshipGameAlgo
{
public:
	GameFromFileAlgo();
	
	~GameFromFileAlgo();
	
	void setBoard(const char** board, int numRows, int numCols) override;
	
	std::pair<int, int> attack() override;
	
	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

	void readBoardFile(const std::string& fileName, char board[BOARD_SIZE][BOARD_SIZE]);
};