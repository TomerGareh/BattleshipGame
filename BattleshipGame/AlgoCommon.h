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
}