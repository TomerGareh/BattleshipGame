#include <stdlib.h>
#include <algorithm>
#include "HuntTargetAlgo.h"
#include "AlgoCommon.h"

HuntTargetAlgo::HuntTargetAlgo() : playerId(-1), boardSize(battleship::NO_MORE_MOVES), visitedBoard(NULL),
								   lastAttackDirection(AttackDirection::InPlace)
{
}

HuntTargetAlgo::~HuntTargetAlgo()
{
	if (visitedBoard != NULL)
	{
		for (int i = 0; i < boardSize.first; ++i)
		{
			delete[] visitedBoard[i];
		}
		delete[] visitedBoard;
	}
}

// To be called only after the initialization of boardSize
void HuntTargetAlgo::allocateVisitedBoard()
{
	visitedBoard = new bool*[boardSize.first];
	for (int i = 0; i < boardSize.first; ++i)
	{
		visitedBoard[i] = new bool[boardSize.second]();	// Init to false
	}
}

void HuntTargetAlgo::setBoard(int player, const char ** board, int numRows, int numCols)
{
	playerId = player;
	boardSize.first = numRows;
	boardSize.second = numCols;
	allocateVisitedBoard();

	// Mark our ships as visited
	for (int i = 0; i < numRows; ++i)
	{
		for (int j = 0; j < numCols; ++j)
		{
			if (board[i][j] != static_cast<char>(battleship::BoardSquare::Empty))
				visitedBoard[i][j] = true;
		}
	}
}

// Ignored by this algorithm 
bool HuntTargetAlgo::init(const string& path)
{
	return true;
}

// row and col are in the range 1 to board size
bool HuntTargetAlgo::calcHitNext(int& row, int& col, AttackDirection direction, int size)
{
	switch (direction)
	{
	case AttackDirection::Right:
	{
		if ((col + size > boardSize.second) || visitedBoard[row-1][col-1+size])
			return false;
		col += size;
		return true;
	}
	case AttackDirection::Left:
	{
		if ((col - size < 1) || visitedBoard[row-1][col-1-size])
			return false;
		col -= size;
		return true;
	}
	case AttackDirection::Up:
	{
		if ((row - size < 1) || visitedBoard[row-1-size][col-1])
			return false;
		row -= size;
		return true;
	}
	case AttackDirection::Down:
	{
		if ((row + size > boardSize.first) || visitedBoard[row-1+size][col-1])
			return false;
		row += size;
		return true;
	}
	default:
		return false;
	}
}

pair<int, int> HuntTargetAlgo::attack()
{
	int row, col;
	if (hitsMap.empty())	// Hunt mode: we draw a random attack
	{
		do {
			row = rand() % boardSize.first + 1;		// In the range 1 to number of rows
			col = rand() % boardSize.second + 1;	// In the range 1 to number of columns
		} while (visitedBoard[row-1][col-1]);
	}
	else	// Target mode: we try to attack around the hits that we already found
	{
		auto currHit = hitsMap.begin();
		row = currHit->first.first + 1;	// 1 to number of rows
		col = currHit->first.second + 1;	// 1 to number of columns
		if (currHit->second[0] != 0)	// Check if we are those who attacked this square
		{								// If not, attack it first
			bool foundMove = false;
			while (!foundMove)	// We aren't suppose to loop here to infinity 
			{					// If this happens, apparently we have a bug
				vector<int>& directionVec = currHit->second;
				vector<int>::iterator maxDirection = std::max_element(directionVec.begin() + 1, directionVec.end());
				size_t direction = std::distance(directionVec.begin(), maxDirection);
				lastAttackDirection = static_cast<AttackDirection>(direction);
				foundMove = calcHitNext(row, col, lastAttackDirection, (directionVec[direction] + 1));
				if (!foundMove)	// This direction is no longer applicable
					directionVec[direction] = -1;
			}
		}
		else if (visitedBoard[row-1][col-1])	// If we already visited the square, it means that currHit is a
												// redundant hit which was inserted by the other player.
		{										// Therefore we remove it and look another attack.
			hitsMap.erase(currHit);
			return attack();
		}
		else
		{
			lastAttackDirection = AttackDirection::InPlace;
		}
	}

	return pair<int, int>(row, col);
}

// TODO: check if row and col are zero-based or one-based. Currently the code assumes that they are zero-based.
void HuntTargetAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	visitedBoard[row][col] = true;

	if (player == playerId)
	{
		if (hitsMap.empty())	// The attack comes from Hunt mode
		{
			if (result == AttackResult::Hit)	// New Hit
			{
				vector<int> newHitSurround = {1, 0, 0, 0, 0};
				hitsMap.emplace(pair<int, int>(row, col), newHitSurround);
			}
		}
		else	// The attack comes from Target mode
		{		// Update hitsMap
			auto lastHit = hitsMap.begin();
			if (result == AttackResult::Miss)
				lastHit->second[static_cast<int>(lastAttackDirection)] = -1;
			else if (result == AttackResult::Hit)
				lastHit->second[static_cast<int>(lastAttackDirection)] += 1;
			else	// Sink
				hitsMap.erase(lastHit);
		}
	}
	else	// Other player
	{
		if (result != AttackResult::Miss)
		{
			visitedBoard[row][col] = false;	// We do want to attack this square
			vector<int> newHitSurround = {0, 0, 0, 0, 0};
			hitsMap.emplace(pair<int, int>(row, col), newHitSurround);
		}
	}
}

ALGO_API IBattleshipGameAlgo* GetAlgorithm()
{
	return new HuntTargetAlgo();
}