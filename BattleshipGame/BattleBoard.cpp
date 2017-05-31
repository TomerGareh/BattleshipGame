#include "BattleBoard.h"
#include "BoardDataImpl.h"

namespace battleship
{
	#pragma region GamePiece

	ShipType::ShipType(BattleBoardSquare representation, int size, int points):
		_representation(representation),
		_size(size),
		_points(points)
	{
	}

	#pragma endregion
	#pragma region GamePiece

	GamePiece::GamePiece(Coordinate firstPos, const ShipType *const type,
						 PlayerEnum player, Orientation orientation) :
		_firstPos(firstPos),
		_shipType(type),
		_player(player),
		_orient(orientation),
		_lifeLeft(type->_size)
	{
	}

	#pragma endregion
	#pragma region BattleBoard

	// Static members initialization
	const ShipType BattleBoard::RUBBER_BOAT(BattleBoardSquare::RubberBoat, 1, 2);
	const ShipType BattleBoard::ROCKET_SHIP(BattleBoardSquare::RocketShip, 2, 3);
	const ShipType BattleBoard::SUBMARINE(BattleBoardSquare::Submarine, 3, 7 );
	const ShipType BattleBoard::BATTLESHIP(BattleBoardSquare::Battleship, 4, 8 );

	// Ctor
	BattleBoard::BattleBoard(int width, int height, int depth):
		_boardWidth(width),
		_boardHeight(height),
		_boardDepth(depth)
	{
	}

	// Move Ctor
	BattleBoard::BattleBoard(BattleBoard&& other) noexcept:
		_gamePieces(std::move(other._gamePieces)),
		_playerAShipCount(other._playerAShipCount),
		_playerBShipCount(other._playerBShipCount),
		_boardWidth(other._boardWidth),
		_boardHeight(other._boardHeight),
		_boardDepth(other._boardDepth)
	{
	}

	// Move assignment operator
	BattleBoard& BattleBoard::operator= (BattleBoard&& other) noexcept
	{
		_gamePieces = std::move(other._gamePieces);
		_playerAShipCount = other._playerAShipCount;
		_playerBShipCount = other._playerBShipCount;
		_boardWidth = other._boardWidth;
		_boardHeight = other._boardHeight;
		_boardDepth = other._boardDepth;

		other._playerAShipCount = 0;
		other._playerBShipCount = 0;
		other._boardWidth = 0;
		other._boardHeight = 0;
		other._boardDepth = 0;
		return *this;
	}

	// Logic methods

	void BattleBoard::addGamePiece(Coordinate firstPos, const ShipType& shipType,
								   PlayerEnum player, Orientation orientation)
	{
		auto gamePieceVal = std::make_shared<GamePiece>(firstPos, &shipType, player, orientation);

		int deltaCol = (orientation == Orientation::X_AXIS) ? 1 : 0;
		int deltaRow = (orientation == Orientation::Y_AXIS) ? 1 : 0;
		int deltaDepth = (orientation == Orientation::Z_AXIS) ? 1 : 0;
		int colOffset = 0;
		int rowOffset = 0;
		int depthOffset = 0;

		for (int index = 0; index < (shipType._size); index++)
		{
			int curCol = firstPos.col + colOffset;
			int curRow = firstPos.row + rowOffset;
			int curDepth = firstPos.depth + depthOffset;
			Coordinate gamePieceKey{ curRow, curCol, curDepth };
			_gamePieces.emplace(gamePieceKey, gamePieceVal);

			rowOffset += deltaRow;
			colOffset += deltaCol;
			depthOffset += deltaDepth;
		}

		if (player == PlayerEnum::A)
		{
			_playerAShipCount++;
		}
		else
		{
			_playerBShipCount++;
		}
	}

	void BattleBoard::sinkShip(const GamePiece* pieceToRemove)
	{
		int deltaCol = (pieceToRemove->_orient == Orientation::X_AXIS) ? 1 : 0;
		int deltaRow = (pieceToRemove->_orient == Orientation::Y_AXIS) ? 1 : 0;
		int deltaDepth = (pieceToRemove->_orient == Orientation::Z_AXIS) ? 1 : 0;
		int colOffset = 0;
		int rowOffset = 0;
		int depthOffset = 0;

		int pieceSize = pieceToRemove->_shipType->_size;

		for (int index = 0; index < pieceSize; index++)
		{
			int curCol = pieceToRemove->_firstPos.col + colOffset;
			int curRow = pieceToRemove->_firstPos.row + rowOffset;
			int curDepth = pieceToRemove->_firstPos.depth + depthOffset;

			// Remove from game-pieces dictionary
			Coordinate gamePieceKey{ curRow, curCol, curDepth };
			_gamePieces.erase(gamePieceKey);

			colOffset += deltaCol;
			rowOffset += deltaRow;
			depthOffset += deltaDepth;
		}

		// Reduce ship count
		if (pieceToRemove->_player == PlayerEnum::A)
			_playerAShipCount--;
		else
			_playerBShipCount--;
	}

	shared_ptr<const GamePiece> BattleBoard::executeAttack(const Coordinate& target)
	{
		auto dictIter = _gamePieces.find(target);
		shared_ptr<GamePiece> gamePiece = NULL;

		if (dictIter != _gamePieces.end())
		{
			gamePiece = dictIter->second;

			// Ship got hit in this part for the first time, reduce life
			if (gamePiece->_damagedCoords.find(target) == (gamePiece->_damagedCoords.end()))
			{
				gamePiece->_lifeLeft--;
				gamePiece->_damagedCoords.insert(target);
			}	

			if (gamePiece->_lifeLeft == 0)
			{	// No life left for ship, sink it
				sinkShip(gamePiece.get());
			}
		}

		return gamePiece;
	}


	// Getters & Setters

	unique_ptr<BoardData> BattleBoard::getBoardPlayerView(PlayerEnum player) const
	{
		return std::make_unique<BoardDataImpl>(player, &_gamePieces, _boardHeight, _boardWidth, _boardDepth);
	}

	const int BattleBoard::getPlayerAShipCount() const
	{
		return _playerAShipCount;
	}

	const int BattleBoard::getPlayerBShipCount() const
	{
		return _playerBShipCount;
	}

	const PlayerEnum BattleBoard::whichPlayerOwnsSquare(const Coordinate& pos) const
	{
		auto dictIter = _gamePieces.find(pos);

		if (dictIter == _gamePieces.end())
		{
			return PlayerEnum::NONE;
		}
		else
		{
			shared_ptr<GamePiece> gamePiece = dictIter->second;
			return gamePiece->_player;
		}
	}

	/** Returns the board width */
	int BattleBoard::width() const
	{
		return _boardWidth;
	}

	/** Returns the board height */
	int BattleBoard::height() const
	{
		return _boardHeight;
	}

	/** Returns the board depth */
	int BattleBoard::depth() const
	{
		return _boardDepth;
	}

	#pragma endregion
}