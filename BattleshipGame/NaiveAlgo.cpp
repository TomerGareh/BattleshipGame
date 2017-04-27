#include "NaiveAlgo.h"
#include "BattleBoard.h"
#include "AlgoCommon.h"

NaiveAlgo::NaiveAlgo()
{
}


NaiveAlgo::~NaiveAlgo()
{
}


void NaiveAlgo::setBoard(int player, const char** board, int numRows, int numCols)
{
	_visitedSquares = new char*[numRows];
	for (int index = 0; index < numRows; index++)
	{
		_visitedSquares[index] = new char[numCols];

		for (int subIndex = 0; subIndex < numCols; subIndex++)
			_visitedSquares[index][subIndex] = (char)(BoardSquare::Empty);
	}

}

bool NaiveAlgo::init(const string& path)
{

}

std::pair<int, int> NaiveAlgo::attack()
{

}

void NaiveAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{

}
