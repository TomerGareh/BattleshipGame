#include "BattleBoard.h"

namespace battleship
{
	#pragma region GamePiece

	ShipType::ShipType(BoardSquare representation, int size, int points):
		_representation(representation),
		_size(size),
		_points(points)
	{
	}

	#pragma endregion
	#pragma region GamePiece

	GamePiece::GamePiece(int firstRow, int firstCol, const ShipType *const type,
						 PlayerEnum player, Orientation orientation) :
		_firstRow(firstRow),
		_firstCol(firstCol),
		_shipType(type),
		_player(player),
		_orient(orientation),
		_lifeLeft(type->_size)
	{
	}

	#pragma endregion
	#pragma region BattleBoard

	// Static members initialization
	const ShipType BattleBoard::RUBBER_BOAT(BoardSquare::RubberBoat, 1, 2);
	const ShipType BattleBoard::ROCKET_SHIP(BoardSquare::RocketShip, 2, 3);
	const ShipType BattleBoard::SUBMARINE(BoardSquare::Submarine, 3, 7 );
	const ShipType BattleBoard::BATTLESHIP(BoardSquare::Battleship, 4, 8 );

	// Ctor
	BattleBoard::BattleBoard()
	{
		_matrix = new char*[BOARD_SIZE];
		for (int index = 0; index < BOARD_SIZE; index++)
		{
			_matrix[index] = new char[BOARD_SIZE];

			for (int subIndex = 0; subIndex < BOARD_SIZE; subIndex++)
				_matrix[index][subIndex] = (char)(BoardSquare::Empty);
		}
	}

	// Move Ctor
	BattleBoard::BattleBoard(BattleBoard&& other) noexcept:
		_matrix(other._matrix),
		_gamePieces(std::move(other._gamePieces)),
		_playerAShipCount(other._playerAShipCount),
		_playerBShipCount(other._playerBShipCount)
	{
		other._matrix = NULL;
	}

	void BattleBoard::disposeAllocatedResources() noexcept
	{
		for (int index = 0; index < BOARD_SIZE; index++)
			delete[] _matrix[index];
		delete[] _matrix;
	}

	// Move assignment operator
	BattleBoard& BattleBoard::operator= (BattleBoard&& other) noexcept
	{
		if (_matrix != NULL)
		{
			disposeAllocatedResources();
		}

		_matrix = other._matrix;
		_gamePieces = std::move(other._gamePieces);
		_playerAShipCount = other._playerAShipCount;
		_playerBShipCount = other._playerBShipCount;

		other._matrix = NULL;
		other._playerAShipCount = 0;
		other._playerBShipCount = 0;
		return *this;
	}

	// Dtor
	BattleBoard::~BattleBoard() noexcept
	{
		disposeAllocatedResources();
	}


	// Logic methods

	void BattleBoard::initSquare(int row, int col, char type)
	{
		_matrix[row][col] = type;
	}

	void BattleBoard::addGamePiece(int firstRow, int firstCol, const ShipType& shipType,
								   PlayerEnum player, Orientation orientation)
	{
		auto gamePieceVal = std::make_shared<GamePiece>(firstRow, firstCol, &shipType, player, orientation);

		int deltaRow = (orientation == Orientation::VERTICAL) ? 1 : 0;
		int deltaCol = (orientation == Orientation::HORIZONTAL) ? 1 : 0;
		int rowOffset = 0;
		int colOffset = 0;

		for (int index = 0; index < (shipType._size); index++)
		{
			int curRow = firstRow + rowOffset;
			int curCol = firstCol + colOffset;
			auto gamePieceKey = std::make_pair(curRow, curCol);
			_gamePieces.emplace(gamePieceKey, gamePieceVal);

			rowOffset += deltaRow;
			colOffset += deltaCol;
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
		int deltaRow = (pieceToRemove->_orient == Orientation::VERTICAL) ? 1 : 0;
		int deltaCol = (pieceToRemove->_orient == Orientation::HORIZONTAL) ? 1 : 0;
		int rowOffset = 0;
		int colOffset = 0;

		int pieceSize = pieceToRemove->_shipType->_size;

		for (int index = 0; index < pieceSize; index++)
		{
			int curRow = pieceToRemove->_firstRow + rowOffset;
			int curCol = pieceToRemove->_firstCol + colOffset;

			// Remove from game-pieces dictionary
			auto gamePieceKey = std::make_pair(curRow, curCol);
			_gamePieces.erase(gamePieceKey);

			// Remove from game board matrix
			_matrix[curRow][curCol] = (char)BoardSquare::Empty;

			rowOffset += deltaRow;
			colOffset += deltaCol;
		}

		// Reduce ship count
		if (pieceToRemove->_player == PlayerEnum::A)
			_playerAShipCount--;
		else
			_playerBShipCount--;
	}

	shared_ptr<const GamePiece> BattleBoard::executeAttack(pair<int, int> target)
	{
		int coordRow = target.first;
		int coordCol = target.second;
		auto dictIter = _gamePieces.find(target);
		shared_ptr<GamePiece> gamePiece = NULL;

		if (dictIter != _gamePieces.end())
		{
			gamePiece = dictIter->second;

			// Ship got hit in this part for the first time, reduce life
			if (_matrix[coordRow][coordCol] != (char)BoardSquare::Hit)
				gamePiece->_lifeLeft--;

			if (gamePiece->_lifeLeft == 0)
			{	// No life left for ship, sink it
				sinkShip(gamePiece.get());
			}
			else
			{	// Ship hit, but not sinked
				_matrix[coordRow][coordCol] = (char)BoardSquare::Hit;
			}
		}

		return gamePiece;
	}


	// Getters & Setters

	const char** BattleBoard::getBoardMatrix() const
	{
		return const_cast<const char**>(_matrix);
	}

	const int BattleBoard::getPlayerAShipCount() const
	{
		return _playerAShipCount;
	}

	const int BattleBoard::getPlayerBShipCount() const
	{
		return _playerBShipCount;
	}

	const PlayerEnum BattleBoard::whichPlayerOwnsSquare(int row, int col) const
	{
		auto dictIter = _gamePieces.find(std::make_pair(row, col));

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

	#pragma endregion
}