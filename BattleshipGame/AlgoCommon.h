#pragma once

#include <utility>

using std::pair;

namespace battleship
{
	/** A constant that represents no more moves remain for game algorithms */
	const Coordinate NO_MORE_MOVES(-1, -1, -1);

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
		Coordinate operator() (Coordinate move, int numOfRows, int numOfCols, int numOfDepths)
		{
			if ((move.row < 1) || (move.col < 1) || (move.depth < 1) ||
				(move.row > numOfRows) || (move.col > numOfCols) || (move.depth > numOfDepths))
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