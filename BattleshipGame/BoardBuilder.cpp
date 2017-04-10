#include <algorithm>
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

	BoardBuilder::ShipMask::ShipMask(BoardSquare ship)
	{
		mask = std::make_unique<ShipMaskList>();

		switch (ship)
		{
		case BoardSquare::RubberBoat:
			mask->insert(mask->end(), {std::make_tuple(-1, 0, ' '), std::make_tuple(0, 1, ' '), std::make_tuple(1, 0, ' '),
									   std::make_tuple(0, -1, ' ')});
			break;
		case BoardSquare::RocketShip:
			mask->insert(mask->end(), {std::make_tuple(0, 1, (char)ShipType::RocketShip), std::make_tuple(-1, 0, ' '),
									   std::make_tuple(-1, 1, ' '), std::make_tuple(0, 2, ' '), std::make_tuple(1, 1, ' '),
									   std::make_tuple(1, 0, ' '), std::make_tuple(0, -1, ' ')});
			break;
		case BoardSquare::Submarine:
			mask->insert(mask->end(), {std::make_tuple(0, 1, (char)ShipType::Submarine), std::make_tuple(0, 2, (char)ShipType::Submarine),
									   std::make_tuple(-1, 0, ' '), std::make_tuple(-1, 1, ' '), std::make_tuple(-1, 2, ' '),
									   std::make_tuple(0, 3, ' '), std::make_tuple(1, 2, ' '), std::make_tuple(1, 1, ' '),
									   std::make_tuple(1, 0, ' '), std::make_tuple(0, -1, ' ')});
			break;
		case BoardSquare::Battleship:
			mask->insert(mask->end(), {std::make_tuple(0, 1, (char)ShipType::Battleship), std::make_tuple(0, 2, (char)ShipType::Battleship),
									   std::make_tuple(0, 3, (char)ShipType::Battleship), std::make_tuple(-1, 0, ' '),
									   std::make_tuple(-1, 1, ' '), std::make_tuple(-1, 2, ' '), std::make_tuple(-1, 3, ' '),
									   std::make_tuple(0, 4, ' '), std::make_tuple(1, 3, ' '), std::make_tuple(1, 2, ' '),
									   std::make_tuple(1, 1, ' '), std::make_tuple(1, 0, ' '), std::make_tuple(0, -1, ' ')});
			break;
		default:
			mask = NULL;
			break;
		}

		matchSizeHorizontal = 1;
		matchSizeVertical = 1;
		wrongSize = false;
		adjacentShips = false;
	}

	BoardBuilder::ShipMask::~ShipMask()
	{
	}

	bool BoardBuilder::ShipMask::applyMask(const shared_ptr<BattleBoard> board, int row, int col, PlayerEnum player)
	{
		int i, j;
		char currMask;
		bool isHorizontalMask = true;
		bool isVerticalMask = true;
		bool horizontalException, verticalException;

		for (ShipMaskList::const_iterator it = mask->begin(); it != mask->end(); it++)
		{
			i = std::get<0>(*it);
			j = std::get<1>(*it);
			currMask = std::get<2>(*it);
			horizontalException = ((row + i < 0) || (row + i >= BOARD_SIZE) || (col + j < 0) || (col + j >= BOARD_SIZE));
			verticalException = ((row + j < 0) || (row + j >= BOARD_SIZE) || (col + i < 0) || (col + i >= BOARD_SIZE));

			if (horizontalException && verticalException)
			{
				if (currMask != ' ')
				{
					isHorizontalMask = false;
					isVerticalMask = false;
					wrongSize = true;
				}
				continue;
			}

			if (player == PlayerEnum::B)
			{
				currMask = tolower(currMask);
			}
			
			if ((!horizontalException) && (board->_matrix[row + i][col + j] == currMask))
			{
				if (currMask != ' ')
				{
					matchSizeHorizontal++;
				}
			}
			else
			{
				isHorizontalMask = false;
			}
			
			if ((!verticalException) && (board->_matrix[row + j][col + i] == currMask))
			{
				if (currMask != ' ')
				{
					matchSizeVertical++;
				}
			}
			else
			{
				isVerticalMask = false;	
			}
			
			if ((!horizontalException) && (!verticalException)
				&& (board->_matrix[row + i][col + j] != currMask) && (board->_matrix[row + j][col + i] != currMask))
			{
				if (currMask != ' ')
				{
					wrongSize = true;
				}
				else
				{
					adjacentShips = true;
				}
			}
		}

		return (isHorizontalMask || isVerticalMask);
	}

	void BoardBuilder::ShipMask::resetMatchSizes()
	{
		matchSizeHorizontal = 1;
		matchSizeVertical = 1;
	}

	BoardBuilder* BoardBuilder::addPiece(int row, int col, char type)
	{
		_board->initSquare(row, col, type);
		return this;
	}

	void BoardBuilder::markVisitedSquares(bool visitedBoard[BOARD_SIZE][BOARD_SIZE], int row, int col, int size, Orientation orient)
	{
		int deltaRow = (orient == Orientation::VERTICAL) ? 1 : 0;
		int deltaCol = (orient == Orientation::HORIZONTAL) ? 1 : 0;
		for (int i = 0; i < size; i++)
		{
			visitedBoard[row + i*deltaRow][col + i*deltaCol] = true;
		}
	}

	// This function assumes that the board contains only ship characters or space, and not any other character
	bool BoardBuilder::isValidBoard()
	{
		unique_ptr<ShipMask> rubberMask = std::make_unique<ShipMask>(BoardSquare::RubberBoat);
		unique_ptr<ShipMask> rocketMask = std::make_unique<ShipMask>(BoardSquare::RocketShip);
		unique_ptr<ShipMask> submarineMask = std::make_unique<ShipMask>(BoardSquare::Submarine);
		unique_ptr<ShipMask> battleshipMask = std::make_unique<ShipMask>(BoardSquare::Battleship);

		char currSquare;
		PlayerEnum player;
		bool visitedBoard[BOARD_SIZE][BOARD_SIZE];
		bool isMatch;
		int matchSize;
		Orientation orient;
		for (int i = 0; i < BOARD_SIZE; i++)
		{
			for (int j = 0; j < BOARD_SIZE; j++)
			{
				currSquare = _board->_matrix[i][j];
				player = (isupper(currSquare)) ? PlayerEnum::A : PlayerEnum::B;

				switch (currSquare)
				{
				case (char)ShipType::RubberBoat:
					isMatch = rubberMask->applyMask(_board, i, j, player);
					if (rubberMask->matchSizeHorizontal >= rubberMask->matchSizeVertical)
					{
						matchSize = rubberMask->matchSizeHorizontal;
						orient = Orientation::HORIZONTAL;
					}
					else
					{
						matchSize = rubberMask->matchSizeVertical;
						orient = Orientation::VERTICAL;
					}
					rubberMask->resetMatchSizes();
					break;
				case (char)ShipType::RocketShip:
					isMatch = rocketMask->applyMask(_board, i, j, player);
					if (rocketMask->matchSizeHorizontal >= rocketMask->matchSizeVertical)
					{
						matchSize = rocketMask->matchSizeHorizontal;
						orient = Orientation::HORIZONTAL;
					}
					else
					{
						matchSize = rocketMask->matchSizeVertical;
						orient = Orientation::VERTICAL;
					}
					rocketMask->resetMatchSizes();
					break;
				case (char)ShipType::Submarine:
					isMatch = submarineMask->applyMask(_board, i, j, player);
					if (submarineMask->matchSizeHorizontal >= submarineMask->matchSizeVertical)
					{
						matchSize = submarineMask->matchSizeHorizontal;
						orient = Orientation::HORIZONTAL;
					}
					else
					{
						matchSize = submarineMask->matchSizeVertical;
						orient = Orientation::VERTICAL;
					}
					submarineMask->resetMatchSizes();
					break;
				case (char)ShipType::Battleship:
					isMatch = battleshipMask->applyMask(_board, i, j, player);
					if (battleshipMask->matchSizeHorizontal >= battleshipMask->matchSizeVertical)
					{
						matchSize = battleshipMask->matchSizeHorizontal;
						orient = Orientation::HORIZONTAL;
					}
					else
					{
						matchSize = battleshipMask->matchSizeVertical;
						orient = Orientation::VERTICAL;
					}
					battleshipMask->resetMatchSizes();
					break;
				default:
					isMatch = false;
					break;
				}

				if (currSquare != ' ')
				{
					markVisitedSquares(visitedBoard, i, j, matchSize, orient);
				}

				if (isMatch)
				{
					_board->addGamePiece(i, j, matchSize, player, orient);
				}

				//error queue
				//reset flags
				//final answer
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
		// void _board->addGamePiece(int firstRow, int firstCol, int size,
		//								  PlayerEnum player, Orientation orientation)

		// !!!!
		// !!!!
		// !!!!
		// !!!!
		// TODO: Tomer - See this when you merge!!
		// Call: _board->addGamePiece(firstX - 1, firstY - 1 .....)
		// We need to normalize coordinates from 1-BOARD_SIZE to 0-BOARD_SIZE-1 as the array works

		printErrors();

		return errorQueue.empty();	// Empty error queue means the board is valid
	}

	shared_ptr<BattleBoard> BoardBuilder::build()
	{
		bool isBoardValid = validate();
		return isBoardValid ? _board : NULL;
	}
}