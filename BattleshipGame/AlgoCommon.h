#pragma once

#include <utility>
#include <iostream>
#include <string>
#include "IBattleshipGameAlgo.h"

using std::pair;
using std::string;
using std::ostream;

namespace battleship
{
	#pragma region Algorithm Commons

	/** A constant that represents no more moves remain for game algorithms */
	const Coordinate NO_MORE_MOVES { -1, -1, -1 };

	/** Represents legal values for squares on the game board */
	enum class BoardSquare : char
	{
		Empty = ' ',
		RubberBoat = 'B',
		RocketShip = 'P',
		Submarine = 'M',
		Battleship = 'D'
	};

	/** An enum to differentiate the players who put pieces on the board */
	enum class PlayerEnum : int
	{
		A = 0,
		B = 1,
		NONE = 2
	};

	/** A functor for validating attack moves returned by home made algorithms are valid. */
	struct AttackValidator
	{
		Coordinate operator() (const Coordinate& move, int rows, int cols, int depth)
		{
			if ((move.row < 1) || (move.col < 1) || (move.depth < 1) ||
				(move.row > rows) || (move.col > cols) || (move.depth > depth))
			{	// Illegal moves are converted into forfeit
				return NO_MORE_MOVES;
			}
			else
			{
				return move;
			}
		}
	};

	#pragma endregion
	#pragma region Coordinate extensions

	string to_string(Coordinate c) {
		return "(" + std::to_string(c.col) + ", " +
					 std::to_string(c.row) + ", " +
					 std::to_string(c.depth) + ")";
	}

	struct CoordinateHash {
		std::size_t operator()(const Coordinate& c) const {
			return c.row * 7 + c.col * 5 + c.depth * 11;
		}
	};

	ostream& operator<<(ostream& out, const Coordinate& c) {
		return out << to_string(c);
	}

	bool operator==(const Coordinate& c1, const Coordinate& c2) {
		// Required for unordered_map
		return c1.col == c2.col && c1.row == c2.row && c1.depth == c2.depth;
	}

	bool operator<(const Coordinate& c1, const Coordinate& c2) {
		// Required for map
		if (c1.col == c2.col)
		{
			if (c1.row == c2.row)
			{
				return c1.depth < c2.depth;
			}
			return c1.row < c2.row;
		}
		return c1.col < c2.col;
	}

	#pragma endregion
}