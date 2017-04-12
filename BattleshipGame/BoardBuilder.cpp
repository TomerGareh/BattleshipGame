#include <iostream>
#include <algorithm>
#include "BoardBuilder.h"

namespace battleship
{
	BoardBuilder::BoardBuilder()
	{
		_board = std::shared_ptr<BattleBoard>(new BattleBoard());	// Only BoardBuilder can instantiate this class
	}

	BoardBuilder::~BoardBuilder()
	{
	}

	BoardBuilder::BoardInitializeError::BoardInitializeError(ErrorPriorityEnum errorType) :
		_errorPriority(errorType), _msg(getErrorMsg(errorType))
	{
	}

	BoardBuilder::BoardInitializeError::~BoardInitializeError()
	{
	}

	string BoardBuilder::BoardInitializeError::getErrorMsg(ErrorPriorityEnum errorType)
	{
		switch ((int)errorType)
		{
			case (int)ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_B_PLAYER_A:
			{
				return "Wrong size or shape for ship B for player A";
			}
			case (int)ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_P_PLAYER_A:
			{
				return "Wrong size or shape for ship P for player A";
			}
			case (int)ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_M_PLAYER_A:
			{
				return "Wrong size or shape for ship M for player A";
			}
			case (int)ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_D_PLAYER_A:
			{
				return "Wrong size or shape for ship D for player A";
			}
			case (int)ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_B_PLAYER_B:
			{
				return "Wrong size or shape for ship b for player B";
			}
			case (int)ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_P_PLAYER_B:
			{
				return "Wrong size or shape for ship p for player B";
			}
			case (int)ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_M_PLAYER_B:
			{
				return "Wrong size or shape for ship m for player B";
			}
			case (int)ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_D_PLAYER_B:
			{
				return "Wrong size or shape for ship d for player B";
			}
			case (int)ErrorPriorityEnum::TOO_MANY_SHIPS_PLAYER_A:
			{
				return "Too many ships for player A";
			}
			case (int)ErrorPriorityEnum::TOO_FEW_SHIPS_PLAYER_A:
			{
				return "Too few ships for player A";
			}
			case (int)ErrorPriorityEnum::TOO_MANY_SHIPS_PLAYER_B:
			{
				return "Too many ships for player B";
			}
			case (int)ErrorPriorityEnum::TOO_FEW_SHIPS_PLAYER_B:
			{
				return "Too few ships for player B";
			}
			case (int)ErrorPriorityEnum::ADJACENT_SHIPS_ON_BOARD:
			{
				return "Adjacent Ships on Board";
			}
			default:
			{
				break;
			}
		}
	}

	BoardBuilder::ShipMask::ShipMask(BoardSquare ship)
	{
		mask = std::make_unique<ShipMaskList>();

		switch (ship)
		{
			case BoardSquare::RubberBoat:
			{
				mask->insert(mask->end(), { std::make_tuple(-1, 0, (char)BoardSquare::Empty), std::make_tuple(0, 1, (char)BoardSquare::Empty),
										    std::make_tuple(1, 0, (char)BoardSquare::Empty), std::make_tuple(0, -1, (char)BoardSquare::Empty) });
				break;
			}
			case BoardSquare::RocketShip:
			{
				mask->insert(mask->end(), { std::make_tuple(0, 1, (char)BoardSquare::RocketShip),
											std::make_tuple(-1, 0, (char)BoardSquare::Empty),
											std::make_tuple(-1, 1, (char)BoardSquare::Empty),
											std::make_tuple(0, 2, (char)BoardSquare::Empty),
											std::make_tuple(1, 1, (char)BoardSquare::Empty),
											std::make_tuple(1, 0, (char)BoardSquare::Empty),
											std::make_tuple(0, -1, (char)BoardSquare::Empty) });
				break;
			}
			case BoardSquare::Submarine:
			{
				mask->insert(mask->end(), { std::make_tuple(0, 1, (char)BoardSquare::Submarine),
											std::make_tuple(0, 2, (char)BoardSquare::Submarine),
											std::make_tuple(-1, 0, (char)BoardSquare::Empty),
											std::make_tuple(-1, 1, (char)BoardSquare::Empty),
											std::make_tuple(-1, 2, (char)BoardSquare::Empty),
											std::make_tuple(0, 3, (char)BoardSquare::Empty),
											std::make_tuple(1, 2, (char)BoardSquare::Empty),
											std::make_tuple(1, 1, (char)BoardSquare::Empty),
											std::make_tuple(1, 0, (char)BoardSquare::Empty),
											std::make_tuple(0, -1, (char)BoardSquare::Empty) });
				break;
			}
			case BoardSquare::Battleship:
			{
				mask->insert(mask->end(), { std::make_tuple(0, 1, (char)BoardSquare::Battleship),
											std::make_tuple(0, 2, (char)BoardSquare::Battleship),
											std::make_tuple(0, 3, (char)BoardSquare::Battleship),
											std::make_tuple(-1, 0, (char)BoardSquare::Empty),
											std::make_tuple(-1, 1, (char)BoardSquare::Empty),
											std::make_tuple(-1, 2, (char)BoardSquare::Empty),
											std::make_tuple(-1, 3, (char)BoardSquare::Empty),
										    std::make_tuple(0, 4, (char)BoardSquare::Empty),
											std::make_tuple(1, 3, (char)BoardSquare::Empty),
											std::make_tuple(1, 2, (char)BoardSquare::Empty),
										    std::make_tuple(1, 1, (char)BoardSquare::Empty),
											std::make_tuple(1, 0, (char)BoardSquare::Empty),
											std::make_tuple(0, -1, (char)BoardSquare::Empty) });
				break;
			}
			default:
			{
				mask = NULL;
				break;
			}
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

		for (auto maskItem : *mask)
		{
			i = std::get<0>(maskItem);
			j = std::get<1>(maskItem);
			currMask = std::get<2>(maskItem);
			horizontalException = ((row + i < 0) || (row + i >= BOARD_SIZE) || (col + j < 0) || (col + j >= BOARD_SIZE));
			verticalException = ((row + j < 0) || (row + j >= BOARD_SIZE) || (col + i < 0) || (col + i >= BOARD_SIZE));

			if (horizontalException && verticalException)
			{
				if (currMask != (char)BoardSquare::Empty)
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
			
			if (!horizontalException)
			{
				if (board->_matrix[row + i][col + j] == currMask)
				{
					if (currMask != (char)BoardSquare::Empty)
					{
						matchSizeHorizontal++;
					}
				}
				else
				{
					isHorizontalMask = false;
				}
			}
			else
			{
				if (currMask != (char)BoardSquare::Empty)
				{
					isHorizontalMask = false;
				}
			}
			
			if (!verticalException)
			{
				if (board->_matrix[row + j][col + i] == currMask)
				{
					if (currMask != (char)BoardSquare::Empty)
					{
						matchSizeVertical++;
					}
				}
				else
				{
					isVerticalMask = false;
				}
			}
			else
			{
				if (currMask != (char)BoardSquare::Empty)
				{
					isVerticalMask = false;
				}
			}
			
			if ((!horizontalException) && (!verticalException)
				&& (board->_matrix[row + i][col + j] != currMask) && (board->_matrix[row + j][col + i] != currMask))
			{
				if (currMask != (char)BoardSquare::Empty)
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

	void BoardBuilder::ShipMask::resetMaskFlags()
	{
		matchSizeHorizontal = 1;
		matchSizeVertical = 1;
		wrongSize = false;
		adjacentShips = false;
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
	bool BoardBuilder::isValidBoard(set<BoardInitializeError, ErrorPriorityFunction>* errorQueue)
	{
		shared_ptr<ShipMask> rubberMask = std::make_shared<ShipMask>(BoardSquare::RubberBoat);
		shared_ptr<ShipMask> rocketMask = std::make_shared<ShipMask>(BoardSquare::RocketShip);
		shared_ptr<ShipMask> submarineMask = std::make_shared<ShipMask>(BoardSquare::Submarine);
		shared_ptr<ShipMask> battleshipMask = std::make_shared<ShipMask>(BoardSquare::Battleship);
		shared_ptr<ShipMask> currMask = NULL;

		bool validBoard = true;
		char currSquare;
		PlayerEnum player;
		bool visitedBoard[BOARD_SIZE][BOARD_SIZE] = {false};
		bool isMatch;
		int matchSize;
		Orientation orient;
		for (int i = 0; i < BOARD_SIZE; i++)
		{
			for (int j = 0; j < BOARD_SIZE; j++)
			{
				if (visitedBoard[i][j])
				{
					continue;
				}

				currSquare = _board->_matrix[i][j];
				player = (isupper(currSquare)) ? PlayerEnum::A : PlayerEnum::B;
				const ShipType* shipType = NULL;

				switch (toupper(currSquare))
				{
				case (char)BoardSquare::RubberBoat:
				{
					shipType = &BattleBoard::RUBBER_BOAT;
					isMatch = rubberMask->applyMask(_board, i, j, player);
					if (rubberMask->wrongSize)
					{
						if (player == PlayerEnum::A)
						{
							errorQueue->insert(BoardInitializeError(ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_B_PLAYER_A));
						}
						else
						{
							errorQueue->insert(BoardInitializeError(ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_B_PLAYER_B));
						}
					}
					currMask = rubberMask;
					break;
				}
				case (char)BoardSquare::RocketShip:
				{
					shipType = &BattleBoard::ROCKET_SHIP;
					isMatch = rocketMask->applyMask(_board, i, j, player);
					if (rocketMask->wrongSize)
					{
						if (player == PlayerEnum::A)
						{
							errorQueue->insert(BoardInitializeError(ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_P_PLAYER_A));
						}
						else
						{
							errorQueue->insert(BoardInitializeError(ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_P_PLAYER_B));
						}
					}
					currMask = rocketMask;
					break;
				}
				case (char)BoardSquare::Submarine:
				{
					shipType = &BattleBoard::SUBMARINE;
					isMatch = submarineMask->applyMask(_board, i, j, player);
					if (submarineMask->wrongSize)
					{
						if (player == PlayerEnum::A)
						{
							errorQueue->insert(BoardInitializeError(ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_M_PLAYER_A));
						}
						else
						{
							errorQueue->insert(BoardInitializeError(ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_M_PLAYER_B));
						}
					}
					currMask = submarineMask;
					break;
				}
				case (char)BoardSquare::Battleship:
				{
					shipType = &BattleBoard::BATTLESHIP;
					isMatch = battleshipMask->applyMask(_board, i, j, player);
					if (battleshipMask->wrongSize)
					{
						if (player == PlayerEnum::A)
						{
							errorQueue->insert(BoardInitializeError(ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_D_PLAYER_A));
						}
						else
						{
							errorQueue->insert(BoardInitializeError(ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_D_PLAYER_B));
						}
					}
					currMask = battleshipMask;
					break;
				}
				default:
					break;
				}

				if (currSquare != (char)BoardSquare::Empty)
				{
					if (currMask->matchSizeHorizontal >= currMask->matchSizeVertical)
					{
						matchSize = currMask->matchSizeHorizontal;
						orient = Orientation::HORIZONTAL;
					}
					else
					{
						matchSize = currMask->matchSizeVertical;
						orient = Orientation::VERTICAL;
					}

					markVisitedSquares(visitedBoard, i, j, matchSize, orient);
					
					if (isMatch)
					{
						_board->addGamePiece(i, j, *shipType, player, orient);
					}
					else
					{
						validBoard = false;
						if (currMask->adjacentShips)
						{
							errorQueue->insert(BoardInitializeError(ErrorPriorityEnum::ADJACENT_SHIPS_ON_BOARD));
						}
					}

					currMask->resetMaskFlags();
				}
			}
		}

		const int numOfShipsA = _board->getPlayerAShipCount();
		const int numOfShipsB = _board->getPlayerBShipCount();
		if (numOfShipsA > NUM_OF_SHIPS_PER_PLAYER)
		{
			validBoard = false;
			errorQueue->insert(BoardInitializeError(ErrorPriorityEnum::TOO_MANY_SHIPS_PLAYER_A));
		}
		else if (numOfShipsA < NUM_OF_SHIPS_PER_PLAYER)
		{
			validBoard = false;
			errorQueue->insert(BoardInitializeError(ErrorPriorityEnum::TOO_FEW_SHIPS_PLAYER_A));
		}
		if (numOfShipsB > NUM_OF_SHIPS_PER_PLAYER)
		{
			validBoard = false;
			errorQueue->insert(BoardInitializeError(ErrorPriorityEnum::TOO_MANY_SHIPS_PLAYER_B));
		}
		else if (numOfShipsB < NUM_OF_SHIPS_PER_PLAYER)
		{
			validBoard = false;
			errorQueue->insert(BoardInitializeError(ErrorPriorityEnum::TOO_FEW_SHIPS_PLAYER_B));
		}

		return validBoard;
	}

	void BoardBuilder::printErrors(set<BoardInitializeError, ErrorPriorityFunction>* errorQueue)
	{
		for (auto err : *errorQueue)
		{
			std::cout << err.getMsg() << std::endl;
		}
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
		
		// Call validation process here, add errors to errorQueue
		bool validBoard = isValidBoard(&errorQueue);

		printErrors(&errorQueue);

		return validBoard;
	}

	shared_ptr<BattleBoard> BoardBuilder::build()
	{
		bool isBoardValid = validate();
		return isBoardValid ? _board : NULL;
	}
}