#include "BattleBoard.h"

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
	const ShipType BattleBoard::RUBBER_BOAT(BattleBoardSquare::RubberBoat, 1, 2);
	const ShipType BattleBoard::ROCKET_SHIP(BattleBoardSquare::RocketShip, 2, 3);
	const ShipType BattleBoard::SUBMARINE(BattleBoardSquare::Submarine, 3, 7 );
	const ShipType BattleBoard::BATTLESHIP(BattleBoardSquare::Battleship, 4, 8 );

	// Ctor
	BattleBoard::BattleBoard()
	{
		_matrix = new char*[BOARD_SIZE];
		for (int index = 0; index < BOARD_SIZE; index++)
		{
			_matrix[index] = new char[BOARD_SIZE];

			for (int subIndex = 0; subIndex < BOARD_SIZE; subIndex++)
			{
				_matrix[index][subIndex] = (char)(BattleBoardSquare::Empty);
			}
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

	void BattleBoard::disposeAllocatedResource(const char* const* resource) noexcept
	{
		if (resource != NULL)
		{
			for (int index = 0; index < BOARD_SIZE; index++)
			{
				delete[] resource[index];
			}
			delete[] resource;
			resource = NULL;
		}
	}

	// Move assignment operator
	BattleBoard& BattleBoard::operator= (BattleBoard&& other) noexcept
	{
		// Add const to type to cope with strict dispose-resource signature
		disposeAllocatedResource(_matrix);

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
		// Add const to type to cope with strict dispose-resource signature
		disposeAllocatedResource(_matrix);
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
			_matrix[curRow][curCol] = (char)BattleBoardSquare::Empty;

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
			if (_matrix[coordRow][coordCol] != (char)BattleBoardSquare::Hit)
				gamePiece->_lifeLeft--;

			if (gamePiece->_lifeLeft == 0)
			{	// No life left for ship, sink it
				sinkShip(gamePiece.get());
			}
			else
			{	// Ship hit, but not sinked
				_matrix[coordRow][coordCol] = (char)BattleBoardSquare::Hit;
			}
		}

		return gamePiece;
	}


	// Getters & Setters

	const char** BattleBoard::getBoard() const
	{
		return const_cast<const char**>(_matrix);
	}

	TempBoardView BattleBoard::getBoardPlayerView(PlayerEnum player) const
	{
		bool isPlayerA = false;

		if (player == PlayerEnum::A)
			isPlayerA = true;
		else if (player == PlayerEnum::B)
			isPlayerA = false;
		else
			return NULL;

		char** playerMat = new char*[BOARD_SIZE];
		
		for (int index = 0; index < BOARD_SIZE; index++)
		{
			playerMat[index] = new char[BOARD_SIZE];

			for (int subIndex = 0; subIndex < BOARD_SIZE; subIndex++)
			{
				char val = _matrix[index][subIndex];

				if ((val != static_cast<char>(BattleBoardSquare::Empty)) &&
					((!isPlayerA && isupper(val)) || (isPlayerA && !isupper(val))))
				{ // Hide enemy ships
					playerMat[index][subIndex] = static_cast<char>(BattleBoardSquare::Empty);
				}
				else
				{
					playerMat[index][subIndex] = val;
				}
			}
		}

		// Cast the raw pointer to a unique_ptr with custom deleter.
		// When consumers are done copying data from this board it will be automatically freed,
		// which will dispose the matrix resource allocated by this function.
		// This way we don't have to rely on an external consumer to manage this piece of allocated mem.
		auto deleter = [](const char** ptr) { disposeAllocatedResource(ptr); };
		TempBoardView playerSmartMat(const_cast<const char**>(playerMat), deleter);

		return playerSmartMat;	// Compiler expected to perform auto move
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