#include "BoardBuilder.h"

namespace battleship
{
	BoardBuilder::BoardBuilder()
	{
		_board = std::make_shared<BattleBoard>();	// Only BoardBuilder can instantiate this class
	}

	BoardBuilder::~BoardBuilder()
	{
	}

	BoardBuilder::ShipMask::ShipMask(ShipType ship)
	{
		mask = std::make_unique<ShipMaskList>();

		switch (ship)
		{
		case ShipType::RubberBoat:
			mask->insert(mask->end(), {std::make_tuple(-1, 0, ' '), std::make_tuple(0, 1, ' '), std::make_tuple(1, 0, ' '),
									   std::make_tuple(0, -1, ' ')});
			break;
		case ShipType::RocketShip:
			mask->insert(mask->end(), {std::make_tuple(0, 1, 'P'), std::make_tuple(-1, 0, ' '), std::make_tuple(-1, 1, ' '),
									   std::make_tuple(0, 2, ' '), std::make_tuple(1, 1, ' '), std::make_tuple(1, 0, ' '),
									   std::make_tuple(0, -1, ' ')});
			break;
		case ShipType::Submarine:
			mask->insert(mask->end(), {std::make_tuple(0, 1, 'M'), std::make_tuple(0, 2, 'M'), std::make_tuple(-1, 0, ' '),
									   std::make_tuple(-1, 1, ' '), std::make_tuple(-1, 2, ' '), std::make_tuple(0, 3, ' '),
									   std::make_tuple(1, 2, ' '), std::make_tuple(1, 1, ' '), std::make_tuple(1, 0, ' '),
									   std::make_tuple(0, -1, ' ')});
			break;
		case ShipType::Battleship:
			mask->insert(mask->end(), {std::make_tuple(0, 1, 'D'), std::make_tuple(0, 2, 'D'), std::make_tuple(0, 3, 'D'),
									   std::make_tuple(-1, 0, ' '), std::make_tuple(-1, 1, ' '), std::make_tuple(-1, 2, ' '),
									   std::make_tuple(-1, 3, ' '), std::make_tuple(0, 4, ' '), std::make_tuple(1, 3, ' '),
									   std::make_tuple(1, 2, ' '), std::make_tuple(1, 1, ' '), std::make_tuple(1, 0, ' '),
									   std::make_tuple(0, -1, ' ')});
			break;
		default:
			mask = NULL;
			break;
		}
	}

	BoardBuilder::ShipMask::~ShipMask()
	{
	}

	bool BoardBuilder::ShipMask::applyMask(const char board[BOARD_SIZE][BOARD_SIZE], std::tuple<int, int> pos, bool isPlayerA,
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

	BoardBuilder* BoardBuilder::addPiece(int x, int y, char type)
	{
		_board->initSquare(x, y, type);
		return this;
	}

	bool BoardBuilder::isValidBoard()
	{
		char visitedBoard[BOARD_SIZE][BOARD_SIZE];

		unique_ptr<ShipMask> rubberMask = std::make_unique<ShipMask>(ShipType::RubberBoat);
		unique_ptr<ShipMask> rocketMask = std::make_unique<ShipMask>(ShipType::RocketShip);
		unique_ptr<ShipMask> submarineMask = std::make_unique<ShipMask>(ShipType::Submarine);
		unique_ptr<ShipMask> battleshipMask = std::make_unique<ShipMask>(ShipType::Battleship);

		int numOfShipsA = 0;
		int numOfShipsB = 0;

		for (int i = 0; i < BOARD_SIZE; i++)
		{
			for (int j = 0; j < BOARD_SIZE; j++)
			{
				if (isupper(_board->matrix[i][j]))
				{

				}
			}
		}

		return true;
	}

	void BoardBuilder::printErrors()
	{
		// TODO: Check if there are errors and if so print them
	}

	bool BoardBuilder::validate()
	{
		ErrorPriorityFunction sortFunc = [](const BoardInitializeError& err1, const BoardInitializeError& err2)
		{
			return err1.getPriority() < err2.getPriority();
		};

		// Initialize a "sorted set" which functions as a queue that automatically sorts errors by priority.
		// This is a set so errors can only be repeated once.
		set<BoardInitializeError, ErrorPriorityFunction> errorQueue(sortFunc);
		
		// TODO: Call validation process here, add errors to errorQueue

		// TODO: when a game piece is verified, add it to the board using this api
		// void _board->addGamePiece(int firstX, int firstY, int size,
		//								  PlayerEnum player, Orientation orientation)

		printErrors();

		return errorQueue.empty();	// Empty error queue means the board is valid
	}

	shared_ptr<BattleBoard> BoardBuilder::build()
	{
		bool isBoardValid = validate();
		return isBoardValid ? _board : NULL;
	}
}