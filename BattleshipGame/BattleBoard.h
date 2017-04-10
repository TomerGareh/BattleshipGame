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
	const pair<int, int> FORFEIT = std::make_pair<int, int>(-1, -1);

	/* Forward Declarations */
	class BoardBuilder;

	/* Type defs */
	using GamePiecesDict = unordered_map<pair<int, int>, shared_ptr<GamePiece>>;

	/* Enums & consts */
	enum class BoardSquare : char
	{
		Empty = ' ',
		Hit = '*',
		RubberBoat = 'B',
		RocketShip = 'P',
		Submarine = 'M',
		Battleship = 'D'
	};

	struct ShipType
	{
		BoardSquare _representation;
		int _size;
		int _points;

		virtual ~ShipType() = default;

		friend class BattleBoard;	// The only class who is allowed to instantiate ShipTypes

	private:
		ShipType(BoardSquare representation, int size, int points);
		ShipType(ShipType const&) = delete;	// Disable copying
		ShipType& operator=(ShipType const&) = delete;	// Disable copying (assignment)
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
		const ShipType *const _shipType;
		Orientation _orient = Orientation::HORIZONTAL;
		PlayerEnum _player = PlayerEnum::A;

		int _lifeLeft = 0;

		GamePiece(int firstRow, int firstCol, const ShipType *const type, PlayerEnum player, Orientation orientation);
		virtual ~GamePiece() = default;
	};

	class BattleBoard
	{
	public:
		virtual ~BattleBoard();

		const shared_ptr<const GamePiece> executeAttack(pair<int, int> target);

		const char** getBoardMatrix() const;
		const int getPlayerAShipCount() const;
		const int getPlayerBShipCount() const;

		friend class BoardBuilder;

	private:
		// Unique instances of 4 known ship types
		static const ShipType RUBBER_BOAT;
		static const ShipType ROCKET_SHIP;
		static const ShipType SUBMARINE;
		static const ShipType BATTLESHIP;

		char** _matrix = NULL;
		GamePiecesDict _gamePieces;
		int _playerAShipCount = 0;
		int _playerBShipCount = 0;

		BattleBoard();

		/* Edits the board, without adding any game-pieces */
		void initSquare(int row, int col, char type);

		/* Called when the board-matrix is initialized, to assemble game pieces list */
		void addGamePiece(int firstRow, int firstCol, const ShipType& shipType,
						  PlayerEnum player, Orientation orientation);

		void sinkShip(GamePiece* pieceToRemove);
	};
}