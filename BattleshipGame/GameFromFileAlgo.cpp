#include "GameFromFileAlgo.h"

GameFromFileAlgo::GameFromFileAlgo()
{
}

GameFromFileAlgo::~GameFromFileAlgo()
{
}

void GameFromFileAlgo::setBoard(const char** board, int numRows, int numCols)
{
	// Ignored by the read from file logic
}

std::pair<int, int> GameFromFileAlgo::attack()
{
	// Execute the next step from the attack file
	return std::pair<int, int>();
}

void GameFromFileAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
}