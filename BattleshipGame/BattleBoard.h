#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

using std::string;
using std::unordered_map;
using std::pair;
using std::shared_ptr;

namespace battleship
{
	const size_t BOARD_SIZE = 10;

	/* Forward Declarations */
	class BoardBuilder;

	using GamePiecesDict = unordered_map<pair<int, int>, shared_ptr<GamePiece>>;

	enum class Orientation: bool
	{
		HORIZONTAL,
		VERTICAL
	};

	enum class PlayerEnum : bool
	{
		A,
		B
	};

	struct GamePiece
	{
	public:
		int _firstX = 0;
		int _firstY = 0;
		int _size = 0;
		Orientation _orient = Orientation::HORIZONTAL;
		PlayerEnum _player = PlayerEnum::A;

		int _lifeLeft = 0;

		GamePiece(int firstX, int firstY, int size, PlayerEnum player, Orientation orientation);
		virtual ~GamePiece();
	};

	class BattleBoard
	{
	public:
		char _matrix[BOARD_SIZE][BOARD_SIZE] = { 0 };
		GamePiecesDict _gamePieces;

		virtual ~BattleBoard();

		friend class BoardBuilder;

	private:
		BattleBoard();

		/* Edits the board, without adding any game-pieces */
		void initSquare(int x, int y, char type);

		/* Called when the board-matrix is initialized, to assemble game pieces list */
		void addGamePiece(int firstX, int firstY, int size, PlayerEnum player, Orientation orientation);
	};
}