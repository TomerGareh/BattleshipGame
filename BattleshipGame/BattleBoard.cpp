#include "BattleBoard.h"

BattleBoard::BattleBoard()
{
}

BattleBoard::~BattleBoard()
{
	if (matrix != NULL) {
		delete[] matrix;
	}
}