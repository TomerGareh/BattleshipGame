#pragma once

#include <memory>
#include <string>
#include <map>
#include <utility>

using std::string;
using std::map;
using std::pair;
using std::shared_ptr;

namespace battleship
{
	/* -- Constants -- */
	const size_t BOARD_SIZE = 10;

	/** A special move returned by players when they choose to forfeit the game (no more moves) */
	const pair<int, int> FORFEIT = std::make_pair<int, int>(-1, -1);

	/* -- Forward Declarations -- */
	class BoardBuilder;
	struct GamePiece;

	/* -- Type defs -- */
	using GamePiecesDict = map<pair<int, int>, shared_ptr<GamePiece>>;

	/* -- Enums & consts -- */

	/** Represents legal values for squares on the game board (including legaly visual values) */
	enum class BoardSquare : char
	{
		Empty = ' ',
		Hit = '*',
		Bombared = '@',
		RubberBoat = 'B',
		RocketShip = 'P',
		Submarine = 'M',
		Battleship = 'D'
	};

	/** Contains data of a ship type, such as size, points, according to game rules */
	struct ShipType
	{
		BoardSquare _representation; // Visual representation of this ship type (case-insensitive)
		int _size;	// Number of squares this ship type occupies
		int _points;	// Number of points a player gets for sinking this ship type

		virtual ~ShipType() = default;

		friend class BattleBoard;	// The only class who is allowed to instantiate ShipTypes

	private:
		ShipType(BoardSquare representation, int size, int points);
		ShipType(ShipType const&) = delete;	// Disable copying
		ShipType& operator=(ShipType const&) = delete;	// Disable copying (assignment)
	};

	/** Orientation of the ship on the battle board */
	enum class Orientation: bool
	{
		HORIZONTAL,
		VERTICAL
	};

	/** An enum to differentiate the players who put pieces on the board */
	enum class PlayerEnum : bool
	{
		A,
		B,
		NONE
	};

	/** 
	 * Represents a single game-piece in the game.
	 * The battle board contains visual data and logical data, this is the logical data representation,
	 * such as how many hit points a ship has left, which player owns it, and so on.
	 */
	struct GamePiece
	{
	public:
		int _firstRow = 0;					// Top left x coordinate of the game piece
		int _firstCol = 0;					// Top left y coordinate of the game piece
		const ShipType *const _shipType;	// A predefined ship-type for this game piece
		Orientation _orient = Orientation::HORIZONTAL;	// How the piece is positioned on board
		PlayerEnum _player = PlayerEnum::A;	// Which player owns the game piece

		int _lifeLeft = 0;	// How many more hits can the game piece sustain

		GamePiece(int firstRow, int firstCol, const ShipType *const type, PlayerEnum player, Orientation orientation);
		virtual ~GamePiece() = default;
	};

	/** Represents the battleship game-board, including all visual and logical data contained within.
	 *  The BattleBoard is expected to get built with a BoardBuilder object, and is kept alive during 
	 *  a game session to hold all game data following the players' moves.
	 */
	class BattleBoard
	{
	public:
		virtual ~BattleBoard();

		/** Apply a player's move on the board, changing a ship's status in case it has been hit.
		 *  Returns NULL if the attack have missed a ship.
		 *  Otherwise returns the status of the game-piece representing the ship that has been hurt,
		 *	after the attack was taken into consideration.
		 */
		const shared_ptr<const GamePiece> executeAttack(pair<int, int> target);

		/** Returns the matrix that represents the visual data in each game square on the board.
		 *  Coordinates are defined in the range [0, BOARD_SIZE-1]
		 */
		const char** getBoardMatrix() const;

		/** Returns the number of ships for player A */
		const int getPlayerAShipCount() const;

		/** Returns the number of ships for player B */
		const int getPlayerBShipCount() const;

		/** Returns the PlayerEnum of the player who owns the piece in square (row, col).
		 *	If this is a blank spot, PlayerEnum::None is returned.
		 *  Coordinates are defined in the range [0, BOARD_SIZE-1]
		 */
		const PlayerEnum whichPlayerOwnsSquare(int row, int col) const;

		/** Allows BoardBuilder access to the private constructor, so BoardBuilder is able to produce new
		 *  BattleBoard instances (according to Builder pattern).
		 */
		friend class BoardBuilder;

	private:
		// Unique instances of 4 known ship types
		static const ShipType RUBBER_BOAT;
		static const ShipType ROCKET_SHIP;
		static const ShipType SUBMARINE;
		static const ShipType BATTLESHIP;

		// The matrix which holds the visual data of the board game squares
		char** _matrix = NULL;

		// List of game pieces in the game (active pieces only, sank pieces are removed).
		// Game pieces are indexed in this dictionary by their index on the game board.
		GamePiecesDict _gamePieces;
		int _playerAShipCount = 0;
		int _playerBShipCount = 0;

		BattleBoard();

		/* Edits the board, without adding any game-pieces. This method affects the visual data only.
		 * Coordinates are defined in the range [0, BOARD_SIZE-1]
		 */
		void initSquare(int row, int col, char type);

		/* Called when the board-matrix is initialized, to assemble game pieces list.
		 * This method is expected to get called before the game starts.
		 * Coordinates are defined in the range [0, BOARD_SIZE-1]
		 */
		void addGamePiece(int firstRow, int firstCol, const ShipType& shipType,
						  PlayerEnum player, Orientation orientation);

		/** Applies a move of "sinking" a game-piece, assuming it has been hit enough times.
		 *  The game piece will be removed from the board altogether, and the players ship count will be updated
		 *  accordingly.
		 */
		void sinkShip(GamePiece* pieceToRemove);
	};
}