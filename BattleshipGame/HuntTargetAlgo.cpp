#include <stdlib.h>
#include <algorithm>
#include <numeric>
#include <time.h>
#include "HuntTargetAlgo.h"
#include "AlgoCommon.h"

HuntTargetAlgo::HuntTargetAlgo():  IBattleshipGameAlgo(),
								   playerId(-1), boardSize(battleship::NO_MORE_MOVES), visitedBoard(NULL),
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

bool HuntTargetAlgo::init(const string& path)
{
	srand(time(NULL));
	return true;
}

AttackDirection HuntTargetAlgo::getTargetDirection(targetsMapEntry targetIt)
{
	vector<int>& directionVec = targetIt->second;
	vector<int>::iterator maxDirection = std::max_element(directionVec.begin() + 1, directionVec.end());
	size_t directionInd = std::distance(directionVec.begin(), maxDirection);
	return static_cast<AttackDirection>(directionInd);
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
	else	// Target mode: we try to attack around the targets that we already found
	{
		auto currTarget = targetsMap.begin();
		row = currTarget->first.first + 1;	// 1 to number of rows
		col = currTarget->first.second + 1;	// 1 to number of columns
		bool foundAttack = false;
		while (!foundAttack)
		{
			lastAttackDirection = getTargetDirection(currTarget);
			int directionInt = static_cast<int>(lastAttackDirection);
			int maxDirection = currTarget->second[directionInt];

			if (maxDirection == -1)	// This is a redundant target that came from the other player
			{						// We remove it to avoid infinite loop
				targetsMap.erase(currTarget);
				return attack();
			}
			
			foundAttack = calcTargetNext(row, col, lastAttackDirection, (maxDirection + 1));
			if (!foundAttack)	// This direction is no longer applicable
				currTarget->second[directionInt] = -1;
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

targetsMapEntry HuntTargetAlgo::updateMapOnOtherAttack(int row, int col, AttackResult result)
{
	for (targetsMapEntry targetIt = targetsMap.begin(); targetIt != targetsMap.end(); ++targetIt)
	{
		int targetRow = targetIt->first.first;
		int targetCol = targetIt->first.second;
		AttackDirection direction = getTargetDirection(targetIt);
		int directionInt = static_cast<int>(direction);
		int maxDirection = targetIt->second[directionInt];

		if (((direction == AttackDirection::Right) && ((targetCol + maxDirection + 1) == col)) ||
			((direction == AttackDirection::Left) && ((targetCol - maxDirection - 1) == col)) ||
			((direction == AttackDirection::Up) && ((targetRow - maxDirection - 1) == row)) ||
			((direction == AttackDirection::Down) && ((targetRow + maxDirection + 1) == row)))
		{
			if (result == AttackResult::Miss)
			{
				targetIt->second[directionInt] = -1;
			}
			else
			{
				targetIt->second[directionInt] += 1;
				lastAttackDirection = direction;
			}
			return targetIt;
		}
	}

	return targetsMap.end();
}

pair<int, int> HuntTargetAlgo::advanceInDirection(int row, int col, AttackDirection direction, int size)
{
	switch (direction)
	{
	case AttackDirection::Right:
	{
		col += size;
		break;
	}
	case AttackDirection::Left:
	{
		col -= size;
		break;
	}
	case AttackDirection::Up:
	{
		row -= size;
		break;
	}
	case AttackDirection::Down:
	{
		row += size;
		break;
	}
	default:
		break;
	}

	return pair<int, int>(row, col);
}

void HuntTargetAlgo::removeRedundantTargets(int row, int col, AttackDirection direction)
{
	pair<int, int> advancedRowCol = advanceInDirection(row, col, direction, 1);
	row = advancedRowCol.first;
	col = advancedRowCol.second;

	for (targetsMapEntry targetIt = targetsMap.begin(); targetIt != targetsMap.end(); ++targetIt)
	{
		int currRow = targetIt->first.first;
		int currCol = targetIt->first.second;
		AttackDirection currDirection = getTargetDirection(targetIt);
		advancedRowCol = advanceInDirection(currRow, currCol, currDirection, targetIt->second[static_cast<int>(currDirection)]);

		if (((currRow == row) && (currCol == col)) ||
			((advancedRowCol.first == row) && (advancedRowCol.second == col)))
		{
			targetsMap.erase(targetIt);
			break;
		}
	}
}

void HuntTargetAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	if (targetsMap.empty())	// The attack comes from Hunt mode or other player
	{
		if ((player == playerId) || ((player != playerId) && (!visitedBoard[row-1][col-1])))
		{
			visitedBoard[row-1][col-1] = true;

			if (result == AttackResult::Hit)	// New target
			{
				vector<int> newTargetSurround = {1, 0, 0, 0, 0};
				targetsMap.emplace(pair<int, int>(row-1, col-1), newTargetSurround);
			}
			else if (result == AttackResult::Sink)	// In case of rubber boat
			{
				markRightLeftAsVisited(row-1, col-1);
				markUpDownAsVisited(row-1, col-1);
			}
		}
	}
	else
	{
		auto currTarget = targetsMap.begin();

		// The attack comes from other player
		if (player != playerId)
		{
			if (!visitedBoard[row-1][col-1])
			{
				visitedBoard[row-1][col-1] = true;
				targetsMapEntry updateMapResult = updateMapOnOtherAttack(row-1, col-1, result);	// Search for existing target
				if ((updateMapResult == targetsMap.end()) || (result == AttackResult::Miss))
				{
					if (result == AttackResult::Hit)	// New target
					{
						vector<int> newTargetSurround = {1, 0, 0, 0, 0};
						targetsMap.emplace(pair<int, int>(row-1, col-1), newTargetSurround);
					}
					return;
				}
				currTarget = updateMapResult;
			}
			else
			{
				return;
			}
		}
		else if (result == AttackResult::Miss)	// player == playerId
		{
			currTarget->second[static_cast<int>(lastAttackDirection)] = -1;
		}
		else	// (player == playerId) && (result != AttackResult::Miss)
		{
			currTarget->second[static_cast<int>(lastAttackDirection)] += 1;
		}

		// The attack comes from Target mode (or self attack of the other player)
		visitedBoard[row-1][col-1] = true;

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

			// Mark the other edge as visited
			if ((lastAttackDirection == AttackDirection::Right) && (col-1-currTargetSize >= 0))
				visitedBoard[row-1][col-1-currTargetSize] = true;
			else if ((lastAttackDirection == AttackDirection::Left) && (col-1+currTargetSize < boardSize.second))
				visitedBoard[row-1][col-1+currTargetSize] = true;
			else if ((lastAttackDirection == AttackDirection::Up) && (row-1+currTargetSize < boardSize.first))
				visitedBoard[row-1+currTargetSize][col-1] = true;
			else if ((lastAttackDirection == AttackDirection::Down) && (row-1-currTargetSize >= 0))
				visitedBoard[row-1-currTargetSize][col-1] = true;

			AttackDirection currTargetDirection = getTargetDirection(currTarget);
			targetsMap.erase(currTarget);
			removeRedundantTargets(row-1, col-1, currTargetDirection);
		}
	}
}

ALGO_API IBattleshipGameAlgo* GetAlgorithm()
{
	return new HuntTargetAlgo();
}