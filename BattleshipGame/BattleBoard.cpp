#include "BattleBoard.h"

namespace battleship
{
	#pragma region GamePiece

	GamePiece::GamePiece(int firstRow, int firstCol, int size, PlayerEnum player, Orientation orientation) :
		_firstRow(firstRow),
		_firstCol(firstCol),
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

	void BattleBoard::initSquare(int row, int col, char type)
	{
		_matrix[row][col] = type;
	}

	void BattleBoard::addGamePiece(int firstRow, int firstCol, int size, PlayerEnum player, Orientation orientation)
	{
		auto gamePieceVal = std::make_shared<GamePiece>(firstRow, firstCol, size, player, orientation);

		int deltaRow = (orientation == Orientation::VERTICAL) ? 1 : 0;
		int deltaCol = (orientation == Orientation::HORIZONTAL) ? 1 : 0;
		int rowOffset = 0;
		int colOffset = 0;

		for (int index = 0; index < size; index++)
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