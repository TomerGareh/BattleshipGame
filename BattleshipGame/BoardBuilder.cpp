#include <iostream>
#include "BoardBuilder.h"
#include "Logger.h"

using std::to_string;

namespace battleship
{
	BoardBuilder::BoardBuilder(int width, int height, int depth) : boardWidth(width), boardHeight(height), boardDepth(depth)
	{
	}

	BoardBuilder::~BoardBuilder()
	{
		// Empty since smart pointers take care of deallocation
	}

	BoardBuilder::BoardInitializeError::BoardInitializeError(ErrorPriorityEnum errorType) :
		_msg(getErrorMsg(errorType)), _errorPriority(errorType)
	{
	}

	BoardBuilder::BoardInitializeError::~BoardInitializeError()
	{
	}

	string BoardBuilder::BoardInitializeError::getErrorMsg(ErrorPriorityEnum errorType)
	{
		switch (errorType)
		{
			case ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_B_PLAYER_A:
			{
				return "Wrong size or shape for ship B for player A";
			}
			case ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_P_PLAYER_A:
			{
				return "Wrong size or shape for ship P for player A";
			}
			case ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_M_PLAYER_A:
			{
				return "Wrong size or shape for ship M for player A";
			}
			case ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_D_PLAYER_A:
			{
				return "Wrong size or shape for ship D for player A";
			}
			case ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_B_PLAYER_B:
			{
				return "Wrong size or shape for ship b for player B";
			}
			case ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_P_PLAYER_B:
			{
				return "Wrong size or shape for ship p for player B";
			}
			case ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_M_PLAYER_B:
			{
				return "Wrong size or shape for ship m for player B";
			}
			case ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_D_PLAYER_B:
			{
				return "Wrong size or shape for ship d for player B";
			}
			case ErrorPriorityEnum::ADJACENT_SHIPS_ON_BOARD:
			{
				return "Adjacent Ships on Board";
			}
			case ErrorPriorityEnum::WRONG_SHIP_TYPES_FOR_BOTH_PLAYERS:
			{
				return "Wrong ship types / amount of types for both players";
			}
			case ErrorPriorityEnum::NO_SHIPS_AT_ALL:
			{
				return "No ships at all on board";
			}
			default:
			{
				return "Unknown board error type";	// Should not reach this line
			}
		}
	}

	BoardBuilder::ShipMask::ShipMask(BattleBoardSquare ship) : maskType(ship)
	{
		switch (ship)
		{
			case BattleBoardSquare::RubberBoat:
			{
				mask = {
					{Coordinate(1, 0, 0), BattleBoardSquare::Empty}, {Coordinate(-1, 0, 0), BattleBoardSquare::Empty},
					{Coordinate(0, 1, 0), BattleBoardSquare::Empty}, {Coordinate(0, -1, 0), BattleBoardSquare::Empty},
					{Coordinate(0, 0, 1), BattleBoardSquare::Empty}, {Coordinate(0, 0, -1), BattleBoardSquare::Empty}
				};
				break;
			}
			case BattleBoardSquare::RocketShip:
			{
				mask = {
					{Coordinate(0, 1, 0), BattleBoardSquare::RocketShip},
					{Coordinate(1, 0, 0), BattleBoardSquare::Empty}, {Coordinate(-1, 0, 0), BattleBoardSquare::Empty},
					{Coordinate(1, 1, 0), BattleBoardSquare::Empty}, {Coordinate(-1, 1, 0), BattleBoardSquare::Empty},
					{Coordinate(0, 2, 0), BattleBoardSquare::Empty}, {Coordinate(0, -1, 0), BattleBoardSquare::Empty},
					{Coordinate(0, 0, 1), BattleBoardSquare::Empty}, {Coordinate(0, 0, -1), BattleBoardSquare::Empty},
					{Coordinate(0, 1, 1), BattleBoardSquare::Empty}, {Coordinate(0, 1, -1), BattleBoardSquare::Empty}
				};
				break;
			}
			case BattleBoardSquare::Submarine:
			{
				mask = {
					{Coordinate(0, 1, 0), BattleBoardSquare::Submarine}, {Coordinate(0, 2, 0), BattleBoardSquare::Submarine},
					{Coordinate(1, 0, 0), BattleBoardSquare::Empty}, {Coordinate(-1, 0, 0), BattleBoardSquare::Empty},
					{Coordinate(1, 1, 0), BattleBoardSquare::Empty}, {Coordinate(-1, 1, 0), BattleBoardSquare::Empty},
					{Coordinate(1, 2, 0), BattleBoardSquare::Empty}, {Coordinate(-1, 2, 0), BattleBoardSquare::Empty},
					{Coordinate(0, 3, 0), BattleBoardSquare::Empty}, {Coordinate(0, -1, 0), BattleBoardSquare::Empty},
					{Coordinate(0, 0, 1), BattleBoardSquare::Empty}, {Coordinate(0, 0, -1), BattleBoardSquare::Empty},
					{Coordinate(0, 1, 1), BattleBoardSquare::Empty}, {Coordinate(0, 1, -1), BattleBoardSquare::Empty},
					{Coordinate(0, 2, 1), BattleBoardSquare::Empty}, {Coordinate(0, 2, -1), BattleBoardSquare::Empty}
				};
				break;
			}
			case BattleBoardSquare::Battleship:
			{
				mask = {
					{Coordinate(0, 1, 0), BattleBoardSquare::Battleship}, {Coordinate(0, 2, 0), BattleBoardSquare::Battleship},
					{Coordinate(0, 3, 0), BattleBoardSquare::Battleship},
					{Coordinate(1, 0, 0), BattleBoardSquare::Empty}, {Coordinate(-1, 0, 0), BattleBoardSquare::Empty},
					{Coordinate(1, 1, 0), BattleBoardSquare::Empty}, {Coordinate(-1, 1, 0), BattleBoardSquare::Empty},
					{Coordinate(1, 2, 0), BattleBoardSquare::Empty}, {Coordinate(-1, 2, 0), BattleBoardSquare::Empty},
					{Coordinate(1, 3, 0), BattleBoardSquare::Empty}, {Coordinate(-1, 3, 0), BattleBoardSquare::Empty},
					{Coordinate(0, 4, 0), BattleBoardSquare::Empty}, {Coordinate(0, -1, 0), BattleBoardSquare::Empty},
					{Coordinate(0, 0, 1), BattleBoardSquare::Empty}, {Coordinate(0, 0, -1), BattleBoardSquare::Empty},
					{Coordinate(0, 1, 1), BattleBoardSquare::Empty}, {Coordinate(0, 1, -1), BattleBoardSquare::Empty},
					{Coordinate(0, 2, 1), BattleBoardSquare::Empty}, {Coordinate(0, 2, -1), BattleBoardSquare::Empty},
					{Coordinate(0, 3, 1), BattleBoardSquare::Empty}, {Coordinate(0, 3, -1), BattleBoardSquare::Empty}
				};
				break;
			}
			default:
			{
				mask = {};
				break;
			}
		}

		orient = Orientation::X_AXIS;
		wrongSize = false;
		adjacentShips = false;
	}

	BoardBuilder::ShipMask::~ShipMask()
	{
	}

	void BoardBuilder::ShipMask::applyMaskEntry(char boardSquare, char shipChar, const MaskEntry& maskEntry, bool axisException,
												int& matchSizeAxis, bool& wrongSizeAxis, bool& adjacentShipsAxis)
	{
		char maskChar = static_cast<char>(maskEntry.second);
		if (islower(shipChar))
			maskChar = tolower(maskChar);

		if (!axisException)
		{
			if (boardSquare == maskChar)
			{
				if (maskChar != static_cast<char>(BattleBoardSquare::Empty))
					matchSizeAxis++;
			}
			else
			{
				if ((boardSquare != static_cast<char>(BattleBoardSquare::Empty)) &&	(boardSquare != shipChar))
				{
					if (matchSizeAxis >= (maskEntry.first.col + 1))
						adjacentShipsAxis = true;
				}
				else
				{
					wrongSizeAxis = true;
				}
			}
		}
		else
		{
			if (maskChar != static_cast<char>(BattleBoardSquare::Empty))
				wrongSizeAxis = true;
		}
	}

	bool BoardBuilder::ShipMask::applyMask(const map<Coordinate, char>& boardMap, tuple<int, int, int> boardSize, Coordinate coord,
										   PlayerEnum player)
	{
		char shipChar = (player == PlayerEnum::A) ? static_cast<char>(maskType) : tolower(static_cast<char>(maskType));
		int boardWidth = std::get<0>(boardSize);
		int boardHeight = std::get<1>(boardSize);
		int boardDepths = std::get<2>(boardSize);

		int matchSizeXAxis = 1;
		int matchSizeYAxis = 1;
		int matchSizeZAxis = 1;
		bool wrongSizeXAxis = false;
		bool wrongSizeYAxis = false;
		bool wrongSizeZAxis = false;
		bool adjacentShipsXAxis = false;
		bool adjacentShipsYAxis = false;
		bool adjacentShipsZAxis = false;

		for (const auto& maskEntry : mask)
		{
			int i = maskEntry.first.row;
			int j = maskEntry.first.col;
			int k = maskEntry.first.depth;
			bool XAxisException = ((coord.row + i < 0) || (coord.row + i >= boardHeight) || (coord.col + j < 0) ||
								   (coord.col + j >= boardWidth) || (coord.depth + k < 0) || (coord.depth + k >= boardDepths));
			bool YAxisException = ((coord.row + j < 0) || (coord.row + j >= boardHeight) || (coord.col + i < 0) ||
								   (coord.col + i >= boardWidth) || (coord.depth + k < 0) || (coord.depth + k >= boardDepths));
			bool ZAxisException = ((coord.row + i < 0) || (coord.row + i >= boardHeight) || (coord.col + k < 0) ||
								   (coord.col + k >= boardWidth) || (coord.depth + j < 0) || (coord.depth + j >= boardDepths));
			
			// Check for X_AXIS orientation
			Coordinate XOrientCoord(coord.row + i, coord.col + j, coord.depth + k);
			char currBoardSquare = (boardMap.find(XOrientCoord) != boardMap.end()) ?
									boardMap.at(XOrientCoord) : static_cast<char>(BattleBoardSquare::Empty);
			applyMaskEntry(currBoardSquare, shipChar, maskEntry, XAxisException, matchSizeXAxis, wrongSizeXAxis, adjacentShipsXAxis);

			// Check for Y_AXIS orientation
			Coordinate YOrientCoord(coord.row + j, coord.col + i, coord.depth + k);
			currBoardSquare = (boardMap.find(YOrientCoord) != boardMap.end()) ?
							   boardMap.at(YOrientCoord) : static_cast<char>(BattleBoardSquare::Empty);
			applyMaskEntry(currBoardSquare, shipChar, maskEntry, YAxisException, matchSizeYAxis, wrongSizeYAxis, adjacentShipsYAxis);

			// Check for Z_AXIS orientation
			Coordinate ZOrientCoord(coord.row + i, coord.col + k, coord.depth + j);
			currBoardSquare = (boardMap.find(ZOrientCoord) != boardMap.end()) ?
							   boardMap.at(ZOrientCoord) : static_cast<char>(BattleBoardSquare::Empty);
			applyMaskEntry(currBoardSquare, shipChar, maskEntry, ZAxisException, matchSizeZAxis, wrongSizeZAxis, adjacentShipsZAxis);
		}

		bool isXAxisMask = ((!wrongSizeXAxis) && (!adjacentShipsXAxis));
		bool isYAxisMask = ((!wrongSizeYAxis) && (!adjacentShipsYAxis));
		bool isZAxisMask = ((!wrongSizeZAxis) && (!adjacentShipsZAxis));

		int maxMatchSize;
		if (matchSizeXAxis < matchSizeYAxis)
		{
			maxMatchSize = matchSizeYAxis;
			orient = Orientation::Y_AXIS;
		}
		else
		{
			maxMatchSize = matchSizeXAxis;
			orient = Orientation::X_AXIS;
		}
		if (maxMatchSize < matchSizeZAxis)
		{
			orient = Orientation::Z_AXIS;
		}
		
		wrongSize = (wrongSizeXAxis && wrongSizeYAxis && wrongSizeZAxis);
		
		if ((!wrongSizeXAxis) && wrongSizeYAxis && wrongSizeZAxis)
		{
			adjacentShips = adjacentShipsXAxis;
		}
		else if ((!wrongSizeYAxis) && wrongSizeXAxis && wrongSizeZAxis)
		{
			adjacentShips = adjacentShipsYAxis;
		}
		else if ((!wrongSizeZAxis) && wrongSizeXAxis && wrongSizeYAxis)
		{
			adjacentShips = adjacentShipsZAxis;
		}
		else
		{
			adjacentShips = (adjacentShipsXAxis || adjacentShipsYAxis || adjacentShipsZAxis);
		}

		return (isXAxisMask || isYAxisMask || isZAxisMask);
	}

	void BoardBuilder::ShipMask::resetMaskFlags()
	{
		wrongSize = false;
		adjacentShips = false;
	}

	BoardBuilder* BoardBuilder::addPiece(Coordinate coord, char type)
	{
		boardMap[coord] = type;
		return this;
	}

	void BoardBuilder::markVisitedCoords(unordered_set<Coordinate, CoordinateHash>& coordSet, Coordinate coord)
	{
		char ship = boardMap[coord];	// This coordinate has came from the boardMap, so it indeed exists
		int j = coord.col;
		bool sameCharInRow = true;

		while ((j < boardWidth) && sameCharInRow)
		{
			int i = coord.row;
			bool sameCharInCol = true;
			while ((i < boardHeight) && sameCharInCol)
			{
				int k = coord.depth;
				bool sameCharInDepth = true;
				while ((k < boardDepth) && sameCharInDepth)
				{
					Coordinate currCoord(i, j, k);
					if ((boardMap.find(currCoord) != boardMap.end()) && (boardMap[currCoord] == ship))
					{
						coordSet.insert(currCoord);
					}
					else
					{
						sameCharInDepth = false;
						if (k == coord.depth)
						{
							sameCharInCol = false;
							if (i == coord.row)
								sameCharInRow = false;
						}
					}
					k++;
				}
				i++;
			}
			j++;
		}
	}

	// This function assumes that the board contains only ship characters or space, and not any other character
	bool BoardBuilder::isValidBoard(BattleBoard* board, set<BoardInitializeError, ErrorPriorityFunction>& errorQueue)
	{
		tuple<int, int, int> boardSize = std::make_tuple(boardWidth, boardHeight, boardDepth);
		shared_ptr<ShipMask> rubberMask = std::make_shared<ShipMask>(BattleBoardSquare::RubberBoat);
		shared_ptr<ShipMask> rocketMask = std::make_shared<ShipMask>(BattleBoardSquare::RocketShip);
		shared_ptr<ShipMask> submarineMask = std::make_shared<ShipMask>(BattleBoardSquare::Submarine);
		shared_ptr<ShipMask> battleshipMask = std::make_shared<ShipMask>(BattleBoardSquare::Battleship);
		shared_ptr<ShipMask> currMask;

		bool validBoard = true;
		unordered_set<Coordinate, CoordinateHash> visitedCoords;
		bool isMatch;
		for (const auto& square : boardMap)
		{
			if (visitedCoords.find(square.first) != visitedCoords.end())
				continue;

			PlayerEnum player = (isupper(square.second)) ? PlayerEnum::A : PlayerEnum::B;
			const ShipType* shipType;

			switch (toupper(square.second))
			{
			case static_cast<char>(BattleBoardSquare::RubberBoat) :
			{
				shipType = &BattleBoard::RUBBER_BOAT;
				isMatch = rubberMask->applyMask(boardMap, boardSize, square.first, player);
				if (rubberMask->wrongSize)
				{
					if (player == PlayerEnum::A)
						errorQueue.insert(BoardInitializeError(ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_B_PLAYER_A));
					else
						errorQueue.insert(BoardInitializeError(ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_B_PLAYER_B));
				}
				currMask = rubberMask;
				break;
			}
			case static_cast<char>(BattleBoardSquare::RocketShip) :
			{
				shipType = &BattleBoard::ROCKET_SHIP;
				isMatch = rocketMask->applyMask(boardMap, boardSize, square.first, player);
				if (rocketMask->wrongSize)
				{
					if (player == PlayerEnum::A)
						errorQueue.insert(BoardInitializeError(ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_P_PLAYER_A));
					else
						errorQueue.insert(BoardInitializeError(ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_P_PLAYER_B));
				}
				currMask = rocketMask;
				break;
			}
			case static_cast<char>(BattleBoardSquare::Submarine) :
			{
				shipType = &BattleBoard::SUBMARINE;
				isMatch = submarineMask->applyMask(boardMap, boardSize, square.first, player);
				if (submarineMask->wrongSize)
				{
					if (player == PlayerEnum::A)
						errorQueue.insert(BoardInitializeError(ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_M_PLAYER_A));
					else
						errorQueue.insert(BoardInitializeError(ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_M_PLAYER_B));
				}
				currMask = submarineMask;
				break;
			}
			case static_cast<char>(BattleBoardSquare::Battleship) :
			{
				shipType = &BattleBoard::BATTLESHIP;
				isMatch = battleshipMask->applyMask(boardMap, boardSize, square.first, player);
				if (battleshipMask->wrongSize)
				{
					if (player == PlayerEnum::A)
						errorQueue.insert(BoardInitializeError(ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_D_PLAYER_A));
					else
						errorQueue.insert(BoardInitializeError(ErrorPriorityEnum::WRONG_SIZE_SHAPE_FOR_SHIP_D_PLAYER_B));
				}
				currMask = battleshipMask;
				break;
			}
			default:
				return false;	// Should not reach this line
			}

			markVisitedCoords(visitedCoords, square.first);

			string logMsg = "Ship type " + string(1, static_cast<char>(shipType->_representation)) + " of player " +
							to_string(static_cast<int>(player));

			if (!currMask->wrongSize)
			{
				Logger::getInstance().log(Severity::DEBUG_LEVEL, logMsg + " is valid.");
				board->addGamePiece(square.first, *shipType, player, currMask->orient);
			}
			
			if (!isMatch)
			{
				Logger::getInstance().log(Severity::DEBUG_LEVEL, logMsg + " is invalid.");
				validBoard = false;
				if (currMask->adjacentShips)
					errorQueue.insert(BoardInitializeError(ErrorPriorityEnum::ADJACENT_SHIPS_ON_BOARD));
			}

			currMask->resetMaskFlags();
		}
		
		if ((board->getPlayerAShipCount() == 0) && (board->getPlayerBShipCount() == 0))
		{
			validBoard = false;
			errorQueue.insert(BoardInitializeError(ErrorPriorityEnum::NO_SHIPS_AT_ALL));
		}

		return validBoard;
	}

	void BoardBuilder::printErrors(const set<BoardInitializeError, ErrorPriorityFunction>& errorQueue)
	{
		for (const auto& err : errorQueue)
		{
			Logger::getInstance().log(Severity::WARNING_LEVEL, err.getMsg());
		}
	}

	unique_ptr<BattleBoard> BoardBuilder::build()
	{
		// Only BoardBuilder can instantiate this class - so we must create without make_shared macro
		unique_ptr<BattleBoard> board(new BattleBoard(boardWidth, boardHeight, boardDepth));
		
		ErrorPriorityFunction sortFunc = [](const BoardInitializeError& err1, const BoardInitializeError& err2)
		{
			return err1.getPriority() < err2.getPriority();
		};

		// Initialize a "sorted set" which functions as a queue that automatically sorts errors by priority.
		// This is a set so errors can only be repeated once.
		set<BoardInitializeError, ErrorPriorityFunction> errorQueue(sortFunc);
		
		// Call validation process here, add errors to errorQueue
		bool validBoard = isValidBoard(board.get(), errorQueue);

		printErrors(errorQueue);

		return validBoard ? std::move(board) : NULL;
	}

	shared_ptr<BattleBoard> BoardBuilder::clone(const BattleBoard& prototype)
	{
		// Only BoardBuilder can instantiate this class - so we must create without make_shared macro
		shared_ptr<BattleBoard> board(new BattleBoard(prototype)); // Invoke copy constructor
		return board;
	}
}