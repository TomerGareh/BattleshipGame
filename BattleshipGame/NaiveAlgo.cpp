#include "NaiveAlgo.h"
#include "BattleBoard.h"
#include "AlgoCommon.h"
#include <vector>
#include <iostream>
#include <exception>

using std::vector;
using std::exception;
using std::cerr;
using std::endl;

pair<int, int> NaiveAlgo::getNextTarget()
{
	// Search for next available square not visited from left to right, top to bottom
	while (_visitedSquares[_nextRow][_nextCol] == static_cast<char>(NaiveSquareStatus::Visited))
	{
		_nextCol++;

		// End of line
		if (_nextCol >= _numOfCols)
		{
			_nextRow++;
			_nextCol = 0;

			// End of board
			if (_nextRow >= _numOfRows)
			{
				return battleship::NO_MORE_MOVES; // Fail safe mechanism,
												  // Shouldn't happen, since it means the game should be over by then.
			}
		}
	}

	return std::make_pair(_nextRow + 1, _nextCol + 1);
}

NaiveAlgo::NaiveAlgo():
IBattleshipGameAlgo(),
_visitedSquares(NULL), _numOfRows(0), _numOfCols(0), _nextRow(0), _nextCol(0)
{
}

void NaiveAlgo::disposeAllocatedResources() noexcept
{
	if (_visitedSquares != NULL)
	{
		// If the matrix is initialized, _numOfRows should contain the real dimensions
		// (otherwise it is 0 and we won't get an ArrayOutOfIndexBounds error)
		for (int index = 0; index < _numOfRows; index++)
			delete[] _visitedSquares[index];
		delete[] _visitedSquares;
		_visitedSquares = NULL;
	}
}

NaiveAlgo::~NaiveAlgo()
{
	disposeAllocatedResources();
}


void NaiveAlgo::setBoard(int player, const char** board, int numRows, int numCols)
{
	_numOfCols = numCols;
	_numOfRows = numRows;

	// Keep a temporary list of occupied squares.
	// Due to game rules neighboring squares can't harbor enemy ships so using this vector we'll
	// mark them also as visited
	vector<pair<int, int>> occupiedSquares;

	// Copy the player's board
	_visitedSquares = new char*[numRows];
	for (int index = 0; index < numRows; index++)
	{
		_visitedSquares[index] = new char[numCols];

		for (int subIndex = 0; subIndex < numCols; subIndex++)
		{
			if (board[index][subIndex] != static_cast<char>(battleship::BoardSquare::Empty))
			{	// Ship encountered, don't visit those squares later on
				_visitedSquares[index][subIndex] = static_cast<char>(NaiveSquareStatus::Visited);
				occupiedSquares.push_back(std::make_pair(index, subIndex));
			}
			else
			{	// Empty squares could hide enemy ships within them so we'll have to try and hit them later on
				_visitedSquares[index][subIndex] = static_cast<char>(NaiveSquareStatus::Unknown);
			}
		}
	}

	// Mark neighboring squares to known occupied squares as visited, since by game rules
	// there cannot be an enemy ship in those squares
	for (auto occSquare : occupiedSquares)
	{
		int row = occSquare.first;
		int col = occSquare.second;

		if (col - 1 >= 0)
		{
			_visitedSquares[row][col - 1] = static_cast<char>(NaiveSquareStatus::Visited);
		}
		if (col + 1 < numCols)
		{
			_visitedSquares[row][col + 1] = static_cast<char>(NaiveSquareStatus::Visited);
		}
		if (row - 1 >= 0)
		{
			_visitedSquares[row - 1][col] = static_cast<char>(NaiveSquareStatus::Visited);
		}
		if (row + 1 < numRows)
		{
			_visitedSquares[row + 1][col] = static_cast<char>(NaiveSquareStatus::Visited);
		}	
	}
}

bool NaiveAlgo::init(const string& path)
{
	// No initialization needed by this algorithm
	return true;
}

std::pair<int, int> NaiveAlgo::attack()
{
	try
	{
		auto target = getNextTarget();
		_visitedSquares[target.first - 1][target.second - 1] = static_cast<char>(NaiveSquareStatus::Visited);

		battleship::AttackValidator validator;
		return validator(target, _numOfRows, _numOfCols);
	}
	catch (const exception& e)
	{	// This should be a barrier that stops the app from failing.
		// If the algorithm fails, let it forfeit
		cerr << "Error: NaiveAlgo::attack failed on " << e.what() << endl;
		return battleship::NO_MORE_MOVES;
	}
}

void NaiveAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	// Naive algorithm is oblivious to attack results
}


ALGO_API IBattleshipGameAlgo* GetAlgorithm()
{
	return new NaiveAlgo();
}