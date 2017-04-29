#include <stdlib.h>
#include <algorithm>
#include <numeric>
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

// This function assumes that row and col are inside the board
void HuntTargetAlgo::markRightLeftAsVisited(int row, int col)
{
	if ((col + 1) < boardSize.second)
		visitedBoard[row][col+1] = true;
	if ((col - 1) >= 0)
		visitedBoard[row][col-1] = true;
}

// This function assumes that row and col are inside the board
void HuntTargetAlgo::markUpDownAsVisited(int row, int col)
{
	if ((row - 1) >= 0)
		visitedBoard[row-1][col] = true;
	if ((row + 1) < boardSize.first)
		visitedBoard[row+1][col] = true;
}

void HuntTargetAlgo::setBoard(int player, const char ** board, int numRows, int numCols)
{
	playerId = player;
	boardSize.first = numRows;
	boardSize.second = numCols;
	allocateVisitedBoard();

	// Mark our ships and their surrounding as visited
	for (int i = 0; i < numRows; ++i)
	{
		for (int j = 0; j < numCols; ++j)
		{
			if (board[i][j] != static_cast<char>(battleship::BoardSquare::Empty))
			{
				visitedBoard[i][j] = true;
				markRightLeftAsVisited(i, j);
				markUpDownAsVisited(i, j);
			}
		}
	}
}

// Ignored by this algorithm 
bool HuntTargetAlgo::init(const string& path)
{
	return true;
}

bool HuntTargetAlgo::calcTargetNext(int& row, int& col, AttackDirection direction, int size)
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
	if (targetsMap.empty())	// Hunt mode: we draw a random attack
	{
		do {
			row = rand() % boardSize.first + 1;		// In the range 1 to number of rows
			col = rand() % boardSize.second + 1;	// In the range 1 to number of columns
		} while (visitedBoard[row-1][col-1]);
		lastAttackDirection = AttackDirection::InPlace;
	}
	else	// Target mode: we try to attack around the hits that we already found
	{
		auto currTarget = targetsMap.begin();
		row = currTarget->first.first + 1;	// 1 to number of rows
		col = currTarget->first.second + 1;	// 1 to number of columns
		if (currTarget->second[0] != 0)	// Check if we are those who attacked this square
		{								// If not, attack it first
			bool foundAttack = false;
			while (!foundAttack)	// We aren't suppose to loop here to infinity 
			{					// If this happens, apparently we have a bug
				vector<int>& directionVec = currTarget->second;
				vector<int>::iterator maxDirection = std::max_element(directionVec.begin() + 1, directionVec.end());
				size_t direction = std::distance(directionVec.begin(), maxDirection);
				lastAttackDirection = static_cast<AttackDirection>(direction);
				foundAttack = calcTargetNext(row, col, lastAttackDirection, (directionVec[direction] + 1));
				if (!foundAttack)	// This direction is no longer applicable
					directionVec[direction] = -1;
			}
		}
		else if (visitedBoard[row-1][col-1])	// If we already visited the square, it means that currTarget is a
												// redundant target which was inserted by the other player.
		{										// Therefore we remove it and look for another attack.
			targetsMap.erase(currTarget);
			return attack();
		}
		else
		{
			lastAttackDirection = AttackDirection::InPlace;
		}
	}

	return pair<int, int>(row, col);
}

int HuntTargetAlgo::getTargetSize(vector<int>* targetSurround)
{
	int size = 0;
	for (auto& directionSize : *targetSurround)
	{
		if (directionSize != -1)
			size += directionSize;
	}
	return size;
}

void HuntTargetAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	visitedBoard[row-1][col-1] = true;

	if (player == playerId)
	{
		if (targetsMap.empty())	// The attack comes from Hunt mode
		{
			if (result == AttackResult::Hit)	// New target
			{
				vector<int> newTargetSurround = {1, 0, 0, 0, 0};
				targetsMap.emplace(pair<int, int>(row, col), newTargetSurround);
			}
			else if (result == AttackResult::Sink)	// In case of rubber boat
			{
				markRightLeftAsVisited(row-1, col-1);
				markUpDownAsVisited(row-1, col-1);
			}
		}
		else	// The attack comes from Target mode
		{		// Update targetsMap
			auto currTarget = targetsMap.begin();
			if (result == AttackResult::Miss)
			{
				currTarget->second[static_cast<int>(lastAttackDirection)] = -1;
			}
			else
			{
				currTarget->second[static_cast<int>(lastAttackDirection)] += 1;
				int currTargetSize = getTargetSize(&(currTarget->second));

				if (currTargetSize == 2)	// We know now the orientation so we mark visited squares for the first hit
				{
					if (lastAttackDirection == AttackDirection::Right)
						markUpDownAsVisited(row-1, col-2);
					else if (lastAttackDirection == AttackDirection::Left)
						markUpDownAsVisited(row-1, col);
					else if (lastAttackDirection == AttackDirection::Up)
						markRightLeftAsVisited(row, col-1);
					else if (lastAttackDirection == AttackDirection::Down)
						markRightLeftAsVisited(row-2, col-1);
				}

				if ((result == AttackResult::Hit) && (currTargetSize > 1))
				{
					if ((lastAttackDirection == AttackDirection::Right) || (lastAttackDirection == AttackDirection::Left))
						markUpDownAsVisited(row-1, col-1);
					else if ((lastAttackDirection == AttackDirection::Up) || (lastAttackDirection == AttackDirection::Down))
						markRightLeftAsVisited(row-1, col-1);
				}
				else if (result == AttackResult::Sink)
				{
					markRightLeftAsVisited(row-1, col-1);
					markUpDownAsVisited(row-1, col-1);

					if ((lastAttackDirection == AttackDirection::Right) && (col-1-currTargetSize >= 0))
						visitedBoard[row-1][col-1-currTargetSize] = true;
					else if ((lastAttackDirection == AttackDirection::Left) && (col-1+currTargetSize < boardSize.second))
						visitedBoard[row-1][col-1+currTargetSize] = true;
					else if ((lastAttackDirection == AttackDirection::Up) && (row-1+currTargetSize < boardSize.first))
						visitedBoard[row-1+currTargetSize][col-1] = true;
					else if ((lastAttackDirection == AttackDirection::Down) && (row-1-currTargetSize >= 0))
						visitedBoard[row-1-currTargetSize][col-1] = true;

					targetsMap.erase(currTarget);
				}
			}
		}
	}
	else	// Other player
	{
		if (result != AttackResult::Miss)
		{
			visitedBoard[row-1][col-1] = false;	// We do want to attack this square
			vector<int> newTargetSurround = {0, 0, 0, 0, 0};
			targetsMap.emplace(pair<int, int>(row-1, col-1), newTargetSurround);
		}
	}
}

ALGO_API IBattleshipGameAlgo* GetAlgorithm()
{
	return new HuntTargetAlgo();
}