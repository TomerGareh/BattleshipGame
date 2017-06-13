#include <iostream>
#include "BoardBuilder.h"

namespace battleship
{
	BoardBuilder::BoardBuilder(int width, int height, int depth)
	{
		// Only BoardBuilder can instantiate this class
		_board = std::shared_ptr<BattleBoard>(new BattleBoard(width, height, depth));
	}

	BoardBuilder::~BoardBuilder()
	{
		// Empty since smart pointers take care of deallocation
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
				return "Unknown board error type";	// Should not reach this line
			}
		}
	}

	BoardBuilder::ShipMask::ShipMask(BattleBoardSquare ship)
	{
		maskType = ship;

		mask = std::make_unique<ShipMaskList>();

		switch (ship)
		{
			case BattleBoardSquare::RubberBoat:
			{
				mask->insert(mask->end(), { std::make_tuple(-1, 0, (char)BattleBoardSquare::Empty), std::make_tuple(0, 1, (char)BattleBoardSquare::Empty),
										    std::make_tuple(1, 0, (char)BattleBoardSquare::Empty), std::make_tuple(0, -1, (char)BattleBoardSquare::Empty) });
				break;
			}
			case BattleBoardSquare::RocketShip:
			{
				mask->insert(mask->end(), { std::make_tuple(0, 1, (char)BattleBoardSquare::RocketShip),
											std::make_tuple(-1, 0, (char)BattleBoardSquare::Empty),
											std::make_tuple(-1, 1, (char)BattleBoardSquare::Empty),
											std::make_tuple(0, 2, (char)BattleBoardSquare::Empty),
											std::make_tuple(1, 1, (char)BattleBoardSquare::Empty),
											std::make_tuple(1, 0, (char)BattleBoardSquare::Empty),
											std::make_tuple(0, -1, (char)BattleBoardSquare::Empty) });
				break;
			}
			case BattleBoardSquare::Submarine:
			{
				mask->insert(mask->end(), { std::make_tuple(0, 1, (char)BattleBoardSquare::Submarine),
											std::make_tuple(0, 2, (char)BattleBoardSquare::Submarine),
											std::make_tuple(-1, 0, (char)BattleBoardSquare::Empty),
											std::make_tuple(-1, 1, (char)BattleBoardSquare::Empty),
											std::make_tuple(-1, 2, (char)BattleBoardSquare::Empty),
											std::make_tuple(0, 3, (char)BattleBoardSquare::Empty),
											std::make_tuple(1, 2, (char)BattleBoardSquare::Empty),
											std::make_tuple(1, 1, (char)BattleBoardSquare::Empty),
											std::make_tuple(1, 0, (char)BattleBoardSquare::Empty),
											std::make_tuple(0, -1, (char)BattleBoardSquare::Empty) });
				break;
			}
			case BattleBoardSquare::Battleship:
			{
				mask->insert(mask->end(), { std::make_tuple(0, 1, (char)BattleBoardSquare::Battleship),
											std::make_tuple(0, 2, (char)BattleBoardSquare::Battleship),
											std::make_tuple(0, 3, (char)BattleBoardSquare::Battleship),
											std::make_tuple(-1, 0, (char)BattleBoardSquare::Empty),
											std::make_tuple(-1, 1, (char)BattleBoardSquare::Empty),
											std::make_tuple(-1, 2, (char)BattleBoardSquare::Empty),
											std::make_tuple(-1, 3, (char)BattleBoardSquare::Empty),
										    std::make_tuple(0, 4, (char)BattleBoardSquare::Empty),
											std::make_tuple(1, 3, (char)BattleBoardSquare::Empty),
											std::make_tuple(1, 2, (char)BattleBoardSquare::Empty),
										    std::make_tuple(1, 1, (char)BattleBoardSquare::Empty),
											std::make_tuple(1, 0, (char)BattleBoardSquare::Empty),
											std::make_tuple(0, -1, (char)BattleBoardSquare::Empty) });
				break;
			}
			default:
			{
				mask = NULL;
				break;
			}
		}

		orient = Orientation::HORIZONTAL;
		wrongSize = false;
		adjacentShips = false;
	}

	BoardBuilder::ShipMask::~ShipMask()
	{
	}

	bool BoardBuilder::ShipMask::applyMask(const shared_ptr<BattleBoard> board, int row, int col, PlayerEnum player)
	{
		char currShip = (player == PlayerEnum::A) ? (char)maskType : tolower((char)maskType);
		int i, j;
		char currMask;
		int matchSizeHorizontal = 1;
		int matchSizeVertical = 1;
		bool wrongSizeHorizontal = false;
		bool wrongSizeVertical = false;
		bool adjacentShipsHorizontal = false;
		bool adjacentShipsVertical = false;
		bool horizontalException, verticalException;

		for (auto maskItem : *mask)
		{
			i = std::get<0>(maskItem);
			j = std::get<1>(maskItem);
			currMask = std::get<2>(maskItem);
			horizontalException = ((row + i < 0) || (row + i >= BOARD_SIZE) || (col + j < 0) || (col + j >= BOARD_SIZE));
			verticalException = ((row + j < 0) || (row + j >= BOARD_SIZE) || (col + i < 0) || (col + i >= BOARD_SIZE));

			if (player == PlayerEnum::B)
			{
				currMask = tolower(currMask);
			}
			
			if (!horizontalException)
			{
				if (board->_matrix[row + i][col + j] == currMask)
				{
					if (currMask != (char)BattleBoardSquare::Empty)
						matchSizeHorizontal++;
				}
				else
				{
					if ((board->_matrix[row + i][col + j] != (char)BattleBoardSquare::Empty) && (board->_matrix[row + i][col + j] != currShip))
					{
						if (matchSizeHorizontal >= (j+1))
							adjacentShipsHorizontal = true;
					}
					else
					{
						wrongSizeHorizontal = true;
					}
				}
			}
			else
			{
				if (currMask != (char)BattleBoardSquare::Empty)
					wrongSizeHorizontal = true;
			}
			
			if (!verticalException)
			{
				if (board->_matrix[row + j][col + i] == currMask)
				{
					if (currMask != (char)BattleBoardSquare::Empty)
						matchSizeVertical++;
				}
				else
				{
					if ((board->_matrix[row + j][col + i] != (char)BattleBoardSquare::Empty) && (board->_matrix[row + j][col + i] != currShip))
					{
						if (matchSizeVertical >= (j+1))
							adjacentShipsVertical = true;
					}
					else
					{
						wrongSizeVertical = true;
					}
				}
			}
			else
			{
				if (currMask != (char)BattleBoardSquare::Empty)
					wrongSizeVertical = true;
			}
		}

		bool isHorizontalMask = ((!wrongSizeHorizontal) && (!adjacentShipsHorizontal));
		bool isVerticalMask = ((!wrongSizeVertical) && (!adjacentShipsVertical));

		orient = (matchSizeHorizontal >= matchSizeVertical) ? Orientation::HORIZONTAL : Orientation::VERTICAL;
		
		wrongSize = (wrongSizeHorizontal && wrongSizeVertical);
		
		if (wrongSizeHorizontal && (!wrongSizeVertical))
		{
			adjacentShips = adjacentShipsVertical;
		}
		else if ((!wrongSizeHorizontal) && wrongSizeVertical)
		{
			adjacentShips = adjacentShipsHorizontal;
		}
		else if (((matchSizeHorizontal == 1) && (matchSizeVertical == 1)) 
				|| ((matchSizeHorizontal > 1) && (matchSizeVertical > 1)))
		{
			adjacentShips = adjacentShipsHorizontal || adjacentShipsVertical;
		}
		else
		{
			adjacentShips = (matchSizeHorizontal > 1) ? adjacentShipsHorizontal : adjacentShipsVertical;
		}

		return (isHorizontalMask || isVerticalMask);
	}

	void BoardBuilder::ShipMask::resetMaskFlags()
	{
		wrongSize = false;
		adjacentShips = false;
	}

	BoardBuilder* BoardBuilder::addPiece(int col, int row, int depth, char type)
	{
		_board->initSquare(row, col, type);
		return this;
	}

	void BoardBuilder::markVisitedSquares(bool visitedBoard[BOARD_SIZE][BOARD_SIZE], int row, int col)
	{
		char ship = _board->_matrix[row][col];
		int i = row;
		int j = col;
		bool sameCharInRow = true;
		bool sameCharInCol = true;
		while ((j < BOARD_SIZE) && (sameCharInRow))
		{
			i = row;
			sameCharInCol = true;
			while ((i < BOARD_SIZE) && (sameCharInCol))
			{
				if (_board->_matrix[i][j] == ship)
				{
					visitedBoard[i][j] = true;
				}
				else
				{
					sameCharInCol = false;
					if (i == row)
					{
						sameCharInRow = false;
					}
				}
				i++;
			}
			j++;
		}
	}

	// This function assumes that the board contains only ship characters or space, and not any other character
	bool BoardBuilder::isValidBoard(set<BoardInitializeError, ErrorPriorityFunction>* errorQueue)
	{
		shared_ptr<ShipMask> rubberMask = std::make_shared<ShipMask>(BattleBoardSquare::RubberBoat);
		shared_ptr<ShipMask> rocketMask = std::make_shared<ShipMask>(BattleBoardSquare::RocketShip);
		shared_ptr<ShipMask> submarineMask = std::make_shared<ShipMask>(BattleBoardSquare::Submarine);
		shared_ptr<ShipMask> battleshipMask = std::make_shared<ShipMask>(BattleBoardSquare::Battleship);
		shared_ptr<ShipMask> currMask = NULL;

		bool validBoard = true;
		char currSquare;
		PlayerEnum player;
		bool visitedBoard[BOARD_SIZE][BOARD_SIZE] = {false};
		bool isMatch = false;
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
				case (char)BattleBoardSquare::RubberBoat:
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
				case (char)BattleBoardSquare::RocketShip:
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
				case (char)BattleBoardSquare::Submarine:
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
				case (char)BattleBoardSquare::Battleship:
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

				if (currSquare != (char)BattleBoardSquare::Empty)
				{
					markVisitedSquares(visitedBoard, i, j);
					
					if (!currMask->wrongSize)
					{
						_board->addGamePiece(i, j, *shipType, player, currMask->orient);
					}
					
					if (!isMatch)
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

	unique_ptr<BattleBoard> BoardBuilder::build()
	{
		bool isBoardValid = validate();
		return isBoardValid ? _board : NULL;
	}

	shared_ptr<BattleBoard> clone(const BattleBoard& prototype)
	{
		return std::make_shared<BattleBoard>(prototype); // Invoke copy constructor
	}
}