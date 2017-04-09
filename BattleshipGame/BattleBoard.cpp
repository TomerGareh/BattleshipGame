#include "BattleBoard.h"

namespace battleship
{
	#pragma region GamePiece

	GamePiece::GamePiece(int firstX, int firstY, int size, PlayerEnum player, Orientation orientation) :
		_firstX(firstX),
		_firstY(firstY),
		_size(size),
		_player(player),
		_orient(orientation),
		_lifeLeft(size)
	{
	}

	GamePiece::~GamePiece()
	{
	}

	#pragma endregion
	#pragma region BattleBoard

	BattleBoard::BattleBoard()
	{
		_matrix = new char*[BOARD_SIZE];
		for (int index = 0; index < BOARD_SIZE; index++)
		{
			_matrix[index] = new char[BOARD_SIZE];

			for (int subIndex = 0; subIndex < BOARD_SIZE; subIndex++)
				_matrix[index][subIndex] = (char)(ShipType::Empty);
		}
	}

	BattleBoard::~BattleBoard()
	{
		for (int index = 0; index < BOARD_SIZE; index++)
			delete[] _matrix[index];
		delete[] _matrix;
	}

	void BattleBoard::initSquare(int x, int y, char type)
	{
		_matrix[x][y] = type;
	}

	void BattleBoard::addGamePiece(int firstX, int firstY, int size, PlayerEnum player, Orientation orientation)
	{
		auto gamePieceVal = std::make_shared<GamePiece>(firstX, firstY, size, player, orientation);

		int deltaX = (orientation == Orientation::HORIZONTAL) ? 1 : 0;
		int deltaY = (orientation == Orientation::VERTICAL) ? 1 : 0;
		int xOffset = 0;
		int yOffset = 0;

		for (int index = 0; index < size; index++)
		{
			int curX = firstX + xOffset;
			int curY = firstY + yOffset;
			auto gamePieceKey = std::make_pair(curX, curY);
			_gamePieces.emplace(gamePieceKey, gamePieceVal);

			xOffset += deltaX;
			yOffset += deltaY;
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

	void BattleBoard::sinkShip(GamePiece* pieceToRemove)
	{
		int deltaX = (pieceToRemove->_orient == Orientation::HORIZONTAL) ? 1 : 0;
		int deltaY = (pieceToRemove->_orient == Orientation::VERTICAL) ? 1 : 0;
		int xOffset = 0;
		int yOffset = 0;

		for (int index = 0; index < pieceToRemove->_size; index++)
		{
			int curX = pieceToRemove->_firstX + xOffset;
			int curY = pieceToRemove->_firstY + yOffset;

			// Remove from game-pieces dictionary
			auto gamePieceKey = std::make_pair(curX, curY);
			_gamePieces.erase(gamePieceKey);

			// Remove from game board matrix
			_matrix[curX][curY] = (char)ShipType::Empty;

			xOffset += deltaX;
			yOffset += deltaY;
		}

		// Reduce ship count
		if (pieceToRemove->_player == PlayerEnum::A)
			_playerAShipCount--;
		else
			_playerBShipCount--;
	}

	const shared_ptr<const GamePiece> BattleBoard::executeAttack(pair<int, int> target)
	{
		int coordX = target.first;
		int coordY = target.second;
		auto dictIter = _gamePieces.find(target);
		shared_ptr<GamePiece> gamePiece = NULL;

		if (dictIter != _gamePieces.end())
		{
			gamePiece = dictIter->second;

			// Ship got hit in this part for the first time, reduce life
			if (_matrix[coordX][coordY] != (char)ShipType::Hit)
				gamePiece->_lifeLeft--;

			if (gamePiece->_lifeLeft == 0)
			{	// No life left for ship, sink it
				sinkShip(gamePiece.get());
			}
			else
			{	// Ship hit, but not sinked
				_matrix[coordX][coordY] = (char)ShipType::Hit;
			}
		}

		return gamePiece;
	}

	const char** BattleBoard::getBoardMatrix() const
	{
		return _matrix;
	}

	const int BattleBoard::getPlayerAShipCount() const
	{
		return _playerAShipCount;
	}

	const int BattleBoard::getPlayerBShipCount() const
	{
		return _playerBShipCount
	}

	#pragma endregion
}