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
			_matrix[index] = new char[BOARD_SIZE];
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
			playerAShipCount++;
		}
		else
		{
			playerBShipCount++;
		}
	}

	const char** BattleBoard::getBoardMatrix() const
	{
		return _matrix;
	}

	#pragma endregion
}