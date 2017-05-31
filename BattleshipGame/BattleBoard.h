#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <set>
#include <utility>
#include <functional>
#include "AlgoCommon.h"
#include "IBattleshipGameAlgo.h"

using std::string;
using std::unordered_map;
using std::set;
using std::pair;
using std::unique_ptr;
using std::shared_ptr;
using std::function;

namespace battleship
{
	/* -- Forward Declarations -- */
	class BoardBuilder;
	struct GamePiece;

	/* -- Type defs -- */

	// A mapping type for game pieces by 3d coordinates.
	using GamePiecesDict = unordered_map<Coordinate, shared_ptr<GamePiece>>;

	/* -- Enums & consts -- */

	/** Represents legal values for squares on the game board (including legaly visual values) */
	enum class BattleBoardSquare : char
	{
		Empty = static_cast<char>(BoardSquare::Empty),
		Hit = '*',
		Bombared = '@',
		Sinking = 'X',
		RubberBoat = static_cast<char>(BoardSquare::RubberBoat),
		RocketShip = static_cast<char>(BoardSquare::RocketShip),
		Submarine = static_cast<char>(BoardSquare::Submarine),
		Battleship = static_cast<char>(BoardSquare::Battleship)
	};

	/** Contains data of a ship type, such as size, points, according to game rules */
	struct ShipType
	{
		BattleBoardSquare _representation; // Visual representation of this ship type (case-insensitive)
		int _size;	// Number of squares this ship type occupies
		int _points;	// Number of points a player gets for sinking this ship type

		virtual ~ShipType() = default;

		friend class BattleBoard;	// The only class who is allowed to instantiate ShipTypes

	private:
		ShipType(BattleBoardSquare representation, int size, int points);
		ShipType(ShipType const&) = delete;	// Disable copying
		ShipType& operator=(ShipType const&) = delete;	// Disable copying (assignment)
		ShipType(ShipType&& other) noexcept = delete; // Disable moving
		ShipType& operator= (ShipType&& other) noexcept = delete; // Disable moving (assignment)
	};

	/** Orientation of the ship on the battle board */
	enum class Orientation: bool
	{
		X_AXIS,
		Y_AXIS,
		Z_AXIS
	};

	/** 
	 * Represents a single game-piece in the game.
	 * The battle board contains visual data and logical data, this is the logical data representation,
	 * such as how many hit points a ship has left, which player owns it, and so on.
	 */
	struct GamePiece
	{
	public:
		Coordinate _firstPos; // Top-Left point of the game piece in XY space,
							  // and lowest point in Z index
						      // (essentially this is the lowest coordinnate of the game piece in all dimensions)
		const ShipType *const _shipType;	// A predefined ship-type for this game piece
		Orientation _orient = Orientation::X_AXIS; // How the piece is positioned on board
		PlayerEnum _player = PlayerEnum::A;	// Which player owns the game piece

		int _lifeLeft = 0;	// How many more hits can the game piece sustain

		set<Coordinate> _damagedCoords;
		GamePiece(Coordinate firstPos, const ShipType *const type, PlayerEnum player, Orientation orientation);
		virtual ~GamePiece() = default;
	};

	/** Represents the battleship game-board, including all visual and logical data contained within.
	 *  The BattleBoard is expected to get built with a BoardBuilder object, and is kept alive during 
	 *  a game session to hold all game data following the players' moves.
	 */
	class BattleBoard
	{
	public:

		BattleBoard(BattleBoard&& other) noexcept; // Move constructor
		BattleBoard& operator= (BattleBoard&& other) noexcept; // Move assignment operator
		virtual ~BattleBoard() noexcept = default;

		/** Apply a player's move on the board, changing a ship's status in case it has been hit.
		 *  Returns NULL if the attack have missed a ship.
		 *  Otherwise returns the status of the game-piece representing the ship that has been hurt,
		 *	after the attack was taken into consideration.
		 */
		shared_ptr<const GamePiece> executeAttack(const Coordinate& target);

		/** Returns a view of the board as the player sees it.
		 *	The view is temporary and should not be held by consumers.
		 *	When the view is done being used (object is no longer referenced), it will get automatically
		 *	deallocated
		 */
		unique_ptr<BoardData> getBoardPlayerView(PlayerEnum player) const;

		/** Returns the number of ships for player A */
		const int getPlayerAShipCount() const;

		/** Returns the number of ships for player B */
		const int getPlayerBShipCount() const;

		/** Returns the PlayerEnum of the player who owns the piece in square (row, col).
		 *	If this is a blank spot, PlayerEnum::None is returned.
		 *  Coordinates are defined in the range [0, BOARD_SIZE-1]
		 */
		const PlayerEnum whichPlayerOwnsSquare(const Coordinate& pos) const;

		/** Returns the board width */
		int width() const;

		/** Returns the board height */
		int height() const;

		/** Returns the board depth */
		int depth() const;

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

		// Dimensions of board in x-y-z axes (number of squares)
		int _boardWidth;
		int _boardHeight;
		int _boardDepth;

		// List of game pieces in the game (active pieces only, sank pieces are removed).
		// Game pieces are indexed in this dictionary by their index on the game board.
		GamePiecesDict _gamePieces;
		int _playerAShipCount = 0;
		int _playerBShipCount = 0;

		// BattleBoard default constructor is private, as only the BoardBuilder is expected to instantiate
		// this object type.
		BattleBoard(int width, int height, int depth);

		// BattleBoard contains a pointer to a 2d buffer containing the visual data,
		// we avoid shallow copies of such objects since the new copy will share the same visual data
		// pointer, which is an error. BattleBoards shouldn't be copied around anyway, so we
		// simply disable them.
		BattleBoard(BattleBoard const&) = delete; // Disable copying
		BattleBoard& operator=(BattleBoard const&) = delete; // Disable copying (assignment)

		/* Called when the board is initialized, to assemble game pieces list.
		 * This method is expected to get called before the game starts.
		 * Coordinates are defined in the range [0, BOARD_SIZE-1]
		 */
		void addGamePiece(Coordinate firstPos, const ShipType& shipType,
						  PlayerEnum player, Orientation orientation);

		/** Applies a move of "sinking" a game-piece, assuming it has been hit enough times.
		 *  The game piece will be removed from the board altogether, and the players ship count will be updated
		 *  accordingly.
		 */
		void sinkShip(const GamePiece* pieceToRemove);
	};
}