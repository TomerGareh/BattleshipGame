#pragma once

/** Represents legal values for squares on the game board */
enum class BoardSquare : char
{
	Empty = ' ',
	RubberBoat = 'B',
	RocketShip = 'P',
	Submarine = 'M',
	Battleship = 'D'
};