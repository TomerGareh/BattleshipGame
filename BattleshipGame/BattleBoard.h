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

	enum class ShipType: char
	{
		RubberBoat = 'B',
		RocketShip = 'P',
		Submarine = 'M',
		Battleship = 'D'
	};

	enum class ShipSize : int
	{
		RubberBoatSize = 1,
		RocketShipSize = 2,
		SubmarineSize = 3,
		BattleshipSize = 4
	};

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
		int _firstRow = 0;
		int _firstCol = 0;
		int _size = 0;
		Orientation _orient = Orientation::HORIZONTAL;
		PlayerEnum _player = PlayerEnum::A;

		int _lifeLeft = 0;

		GamePiece(int firstRow, int firstCol, int size, PlayerEnum player, Orientation orientation);
		virtual ~GamePiece();
	};

	class BattleBoard
	{
	public:
		virtual ~BattleBoard();

		const char** getBoardMatrix() const;

		friend class BoardBuilder;

	private:
		char** _matrix = NULL;
		GamePiecesDict _gamePieces;
		int playerAShipCount = 0;
		int playerBShipCount = 0;

		BattleBoard();

		/* Edits the board, without adding any game-pieces */
		void initSquare(int row, int col, char type);

		/* Called when the board-matrix is initialized, to assemble game pieces list */
		void addGamePiece(int firstRow, int firstCol, int size, PlayerEnum player, Orientation orientation);
	};
}