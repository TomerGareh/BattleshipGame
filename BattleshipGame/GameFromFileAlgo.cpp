#include <memory>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cctype>
#include "GameFromFileAlgo.h"

GameFromFileAlgo::GameFromFileAlgo()
{
}

GameFromFileAlgo::~GameFromFileAlgo()
{
}

GameFromFileAlgo::ShipMask::ShipMask(ShipType ship)
{
	mask = std::make_unique<ShipMaskList>(new ShipMaskList());

	switch (ship)
	{
	case ShipType::RubberBoat:
		mask->insert(mask->end(), { std::make_tuple(-1, 0, ' '), std::make_tuple(0, 1, ' '), std::make_tuple(1, 0, ' '),
								   std::make_tuple(0, -1, ' ') });
		break;
	case ShipType::RocketShip:
		mask->insert(mask->end(), { std::make_tuple(0, 1, 'P'), std::make_tuple(-1, 0, ' '), std::make_tuple(-1, 1, ' '),
								   std::make_tuple(0, 2, ' '), std::make_tuple(1, 1, ' '), std::make_tuple(1, 0, ' '),
								   std::make_tuple(0, -1, ' ') });
		break;
	case ShipType::Submarine:
		mask->insert(mask->end(), { std::make_tuple(0, 1, 'M'), std::make_tuple(0, 2, 'M'), std::make_tuple(-1, 0, ' '),
								   std::make_tuple(-1, 1, ' '), std::make_tuple(-1, 2, ' '), std::make_tuple(0, 3, ' '),
								   std::make_tuple(1, 2, ' '), std::make_tuple(1, 1, ' '), std::make_tuple(1, 0, ' '),
								   std::make_tuple(0, -1, ' ') });
		break;
	case ShipType::Battleship:
		mask->insert(mask->end(), { std::make_tuple(0, 1, 'D'), std::make_tuple(0, 2, 'D'), std::make_tuple(0, 3, 'D'),
								   std::make_tuple(-1, 0, ' '), std::make_tuple(-1, 1, ' '), std::make_tuple(-1, 2, ' '),
								   std::make_tuple(-1, 3, ' '), std::make_tuple(0, 4, ' '), std::make_tuple(1, 3, ' '),
								   std::make_tuple(1, 2, ' '), std::make_tuple(1, 1, ' '), std::make_tuple(1, 0, ' '),
								   std::make_tuple(0, -1, ' ') });
		break;
	default:
		mask = NULL;
		break;
	}
}

GameFromFileAlgo::ShipMask::~ShipMask()
{
}

bool GameFromFileAlgo::ShipMask::applyMask(const char board[BOARD_SIZE][BOARD_SIZE], std::tuple<int, int> pos, bool isPlayerA,
										   bool& wrongSize, bool& adjacentShips, bool& horizontalMatch)
{
	int row = std::get<0>(pos);
	int col = std::get<1>(pos);
	int i, j;
	char currMask;
	bool isHorizontalMask = true;
	bool isVerticalMask = true;

	for (ShipMaskList::const_iterator it = mask->begin(); it != mask->end(); it++)
	{
		i = std::get<0>(*it);
		j = std::get<1>(*it);
		currMask = std::get<2>(*it);
		if (!isPlayerA)
		{
			currMask = tolower(currMask);
		}
		if (board[row + i][col + j] != currMask)
		{
			isHorizontalMask = false;
			if ((currMask != ' ') && (board[row + j][col + i] != currMask))
			{
				wrongSize = true;
			}
		}
		if (board[row + j][col + i] != currMask)
		{ 
			isVerticalMask = false;
		}
	}

	horizontalMatch = isHorizontalMask;

	return (isHorizontalMask || isVerticalMask);
}

bool GameFromFileAlgo::isValidBoard(const char board[BOARD_SIZE][BOARD_SIZE])
{
	char visitedBoard[BOARD_SIZE][BOARD_SIZE];

	std::unique_ptr<ShipMask> rubberMask = std::make_unique<ShipMask>(ShipMask(ShipType::RubberBoat));
	std::unique_ptr<ShipMask> rubberMask = std::make_unique<ShipMask>(ShipMask(ShipType::RocketShip));
	std::unique_ptr<ShipMask> rubberMask = std::make_unique<ShipMask>(ShipMask(ShipType::Submarine));
	std::unique_ptr<ShipMask> rubberMask = std::make_unique<ShipMask>(ShipMask(ShipType::Battleship));

	int numOfShipsA = 0;
	int numOfShipsB = 0;

	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (isupper(board[i][j]))
			{

			}
		}
	}

	return true;
}

// This function assumes that the file exists and the file name is valid
void GameFromFileAlgo::readBoardFile(const std::string& fileName, char board[BOARD_SIZE][BOARD_SIZE])
{
	std::ifstream fin(fileName);
	char nextChar;
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			fin.get(nextChar);
			if ((nextChar == '\n') || (nextChar == '\r\n') || (nextChar == EOF))
			{
				break;
			}
			board[i][j] = nextChar;
		}
	}
}

void GameFromFileAlgo::setBoard(const char** board, int numRows, int numCols)
{
}

std::pair<int, int> GameFromFileAlgo::attack()
{
	return std::pair<int, int>();
}

void GameFromFileAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
}