#pragma once

#include <utility>

using std::pair;

namespace battleship
{
	/** A constant that represents no more moves remain for game algorithms */
	const pair<int, int> NO_MORE_MOVES = std::make_pair(-1, -1);

	/** Represents legal values for squares on the game board */
	enum class BoardSquare : char
	{
		Empty = ' ',
		RubberBoat = 'B',
		RocketShip = 'P',
		Submarine = 'M',
		Battleship = 'D'
	};

	/** A functor for validating attack moves returned by home made algorithms are valid. */
	struct AttackValidator
	{
		pair<int, int> operator() (pair<int, int> move, int numOfRows, int numOfCols)
		{
			if ((move.first < 1) || (move.second < 1) ||
				(move.first > numOfRows) || (move.second > numOfCols))
			{	// Illegal moves are converted into forfeit
				return NO_MORE_MOVES;
			}
			else
			{
				return move;
			}
		}
	};
}