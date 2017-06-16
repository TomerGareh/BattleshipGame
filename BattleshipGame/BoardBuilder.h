#pragma once

#include <memory>
#include <tuple>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <functional>
#include "IBattleshipGameAlgo.h"
#include "BattleBoard.h"
#include "AlgoCommon.h"

using std::shared_ptr;
using std::string;
using std::pair;
using std::tuple;
using std::vector;
using std::set;
using std::map;
using std::unordered_set;
using std::function;

namespace battleship
{
	/* List of possible validation error types, with priority values assigned.
	 * Lower index means higher priority when the errors list is outputted.
	 */
	enum class ErrorPriorityEnum
	{
		WRONG_SIZE_SHAPE_FOR_SHIP_B_PLAYER_A,
		WRONG_SIZE_SHAPE_FOR_SHIP_P_PLAYER_A,
		WRONG_SIZE_SHAPE_FOR_SHIP_M_PLAYER_A,
		WRONG_SIZE_SHAPE_FOR_SHIP_D_PLAYER_A,
		WRONG_SIZE_SHAPE_FOR_SHIP_B_PLAYER_B,
		WRONG_SIZE_SHAPE_FOR_SHIP_P_PLAYER_B,
		WRONG_SIZE_SHAPE_FOR_SHIP_M_PLAYER_B,
		WRONG_SIZE_SHAPE_FOR_SHIP_D_PLAYER_B,
		ADJACENT_SHIPS_ON_BOARD,
		WRONG_SHIP_TYPES_FOR_BOTH_PLAYERS,
		NO_SHIPS_AT_ALL
	};

	/** A Builder pattern class, for creating instances of the BattleBoard class.
	 *  BoardBuilder is the only class expected to create BattleBoards, and is responsible for vailidating
	 *	the board before the beginning of a game session.
	 */
	class BoardBuilder
	{
	public:
		/** Creates a new BoardBuilder instance */
		BoardBuilder(int width, int height, int depth);
		virtual ~BoardBuilder();

		BoardBuilder(BoardBuilder const&) = delete;	// Disable copying
		BoardBuilder& operator=(BoardBuilder const&) = delete;	// Disable copying (assignment)
		BoardBuilder(BoardBuilder&& other) noexcept = delete; // Disable moving
		BoardBuilder& operator= (BoardBuilder&& other) noexcept = delete; // Disable moving (assignment)

		/** Represents a validation error that might rise when the board is initialized.
		 *  This object helps keep the errors sorted by priority order, and output the right error message.
		 */
		class BoardInitializeError
		{
		public:
			/** Construct a new error, defined by error type (which determines the message and priority) */
			BoardInitializeError(ErrorPriorityEnum errorType);
			virtual ~BoardInitializeError();
			
			/** Returns the error message string stored for this error */
			const string& getMsg() const { return _msg; };
			
			/** Returns the priority enum of the current error */
			ErrorPriorityEnum getPriority() const { return _errorPriority; };
		private:
			string _msg;
			ErrorPriorityEnum _errorPriority;

			/** A utility function for creating a message string for each error type.
			 *  We expect the compiler to perfrom RVO optimization, so we safely return a string here
			 *	(no copy is expected to be made)
			 */
			static string getErrorMsg(ErrorPriorityEnum errorType);
		};

		// Typedef for error priority function, that compares two errors.
		// Helps the STL containers stay sorted.
		using ErrorPriorityFunction = function<bool(const BoardInitializeError&, const BoardInitializeError&)>;

		/** Defines a value for a single game-square on the battle-board.
		 *  This method will not generate a game-piece in the real BattleBoard object just yet.
		 */
		BoardBuilder* addPiece(Coordinate coord, char type);

		/** Finailize the creation of the BattleBoard.
		 *	Validation occurs here, and logical game pieces data is initialized for the BattleBoard object.
		 *	In the end the constructed BattleBoard instance is returned, or NULL if errors have occured in the process.
		 *	Any validation errors that might occur will be printed by this routine, in descending priority order.
		 */
		unique_ptr<BattleBoard> build();

		/** Creates a new instance of the battle board out of the given prototype.
		 *  Boards will be identical in data, but will not share the same game pieces.
		 */
		static shared_ptr<BattleBoard> clone(const BattleBoard& prototype);

	private:
		/** A helper class for validating the legal formation of game-pieces on the board. */
		class ShipMask
		{
		public:
			BattleBoardSquare maskType;

			/** A map of squares that compose the mask. The coordinate is a relative coordinate. */
			
			vector<pair<Coordinate, BattleBoardSquare>> mask;
			using MaskEntry = pair<Coordinate, BattleBoardSquare>;
			
			Orientation orient;
			bool wrongSize;
			bool adjacentShips;

			/** Constructs a new mask according to ship type (which is represented by the BattleBoardSquare in this case) */
			ShipMask(BattleBoardSquare ship);

			virtual ~ShipMask();

			/** Apply ship mask to the board, and return whether the current formation for the (row, col, depth) square
			 *	is valid.
			 */
			bool applyMask(const map<Coordinate, char>& boardMap, tuple<int, int, int> boardSize, Coordinate coord, PlayerEnum player);

			static void applyMaskEntry(char boardSquare, char shipChar, const MaskEntry& maskEntry, bool axisException,
									   int& matchSizeAxis, bool& wrongSizeAxis, bool& adjacentShipsAxis);

			/** Clean state and prepare for next comparison */
			void resetMaskFlags();

			/** BoardBuilder is the only class which is allowed to instantiate ShipMasks */
			friend class BoardBuilder;
		};

		int boardWidth;
		int boardHeight;
		int boardDepth;

		/** The board itself as a map from Coordinate to char */
		map<Coordinate, char> boardMap;

		/** Mark given squares as already validated */
		void markVisitedCoords(unordered_set<Coordinate, CoordinateHash>& coordSet, Coordinate coord);

		/** Returns true if the BattleBoard contains a legal formation, false if not.
		 *  This function is used by BoardBuilder::validate()
		 */
		bool isValidBoard(BattleBoard* board, set<BoardInitializeError, ErrorPriorityFunction>& errorQueue);

		/** Prints the validation errors in the queue, in descending priority order */
		static void printErrors(const set<BoardInitializeError, ErrorPriorityFunction>& errorQueue);
	};
}