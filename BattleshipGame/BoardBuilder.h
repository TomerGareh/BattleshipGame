#pragma once

#include <memory>
#include <string>
#include <tuple>
#include <list>
#include <set>
#include <functional>
#include "BattleBoard.h"

using std::string;
using std::tuple;
using std::list;
using std::set;
using std::function;
using std::shared_ptr;
using std::unique_ptr;

namespace battleship
{
	/* List of possible validation error types, with priority values assigned.
	 * Lower index means higher priority when the errors list is outputted.
	 */
	enum class ErrorPriorityEnum: int
	{
		WRONG_SIZE_SHAPE_FOR_SHIP_B_PLAYER_A = 0,
		WRONG_SIZE_SHAPE_FOR_SHIP_P_PLAYER_A = 1,
		WRONG_SIZE_SHAPE_FOR_SHIP_M_PLAYER_A = 2,
		WRONG_SIZE_SHAPE_FOR_SHIP_D_PLAYER_A = 3,
		WRONG_SIZE_SHAPE_FOR_SHIP_B_PLAYER_B = 4,
		WRONG_SIZE_SHAPE_FOR_SHIP_P_PLAYER_B = 5,
		WRONG_SIZE_SHAPE_FOR_SHIP_M_PLAYER_B = 6,
		WRONG_SIZE_SHAPE_FOR_SHIP_D_PLAYER_B = 7,
		TOO_MANY_SHIPS_PLAYER_A = 8,
		TOO_FEW_SHIPS_PLAYER_A = 9,
		TOO_MANY_SHIPS_PLAYER_B = 10,
		TOO_FEW_SHIPS_PLAYER_B = 11,
		ADJACENT_SHIPS_ON_BOARD = 12
	};

	/** A Builder pattern class, for creating instances of the BattleBoard class.
	 *  BoardBuilder is the only class expected to create BattleBoards, and is responsible for vailidating
	 *	the board before the beginning of a game session.
	 */
	class BoardBuilder
	{
	public:
		/** Creates a new BoardBuilder instance */
		BoardBuilder();
		virtual ~BoardBuilder();

		/** Represents a validation error that might rise when the board is initialized.
		 *  This object helps keep the errors sorted by priority order, and output the right error message.
		 */
		class BoardInitializeError
		{
		public:
			/** Construct a new error, defined by error type (which determines the message and priority) */
			BoardInitializeError(ErrorPriorityEnum errorType);

			~BoardInitializeError();
			
			/** Returns the error message string stored for this error */
			const string& getMsg() const { return _msg; };
			
			/** Returns the priority enum of the current error */
			const ErrorPriorityEnum getPriority() const { return _errorPriority; };
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
		BoardBuilder* addPiece(int row, int col, char type);

		/** Finailize the creation of the BattleBoard.
		 *	Validation occurs here, and logical game pieces data is initialized for the BattleBoard object.
		 *	In the end the constructed BattleBoard instance is returned, or NULL if errors have occured in the process.
		 *	Any validation errors that might occur will be printed by this routine, in descending priority order.
		 */
		shared_ptr<BattleBoard> build();

	private:
		/** A helper class for validating the legal formation of game-pieces on the board. */
		class ShipMask
		{
		public:
			/** A list of squares that compose the mask*/
			typedef list<tuple<int, int, char>> ShipMaskList;
			typedef unique_ptr<ShipMaskList> ShipMaskListPtr;
			
			ShipMaskListPtr mask;

			int matchSizeHorizontal;
			int matchSizeVertical;
			bool wrongSize;
			bool adjacentShips;

			virtual ~ShipMask();

			/** Apply ship mask to the board, and return whether the current formation for the (row, col) square
			 *	is valid.
			 */
			bool applyMask(const shared_ptr<BattleBoard> board, int row, int col, PlayerEnum player);

			/** Clean state and prepare for next comparison */
			void resetMaskFlags();

		//private:
			/** Constructs a new mask according to ship type (which is represented by the BoardSquare in this case) */
			ShipMask(BoardSquare ship);

			/** BoardBuilder is the only class which is allowed to instantiate ShipMasks */
			friend class BoardBuilder;
		};

		/** The BattleBoard instance being gradually created by this builder object */
		shared_ptr<BattleBoard> _board;

		/** Mark given squares as already validated */
		void markVisitedSquares(bool visitedBoard[BOARD_SIZE][BOARD_SIZE], int row, int col, int size, Orientation orient);

		/** Prepares the error queue, calls isValidBoard to validate the board and prints the generated errors
		 *	in the end of the process.
		 */
		bool validate();

		/** Returns true if the BattleBoard contains a legal formation, false if not.
		 *  This function is used by BoardBuilder::validate()
		 */
		bool isValidBoard(set<BoardInitializeError, ErrorPriorityFunction>* errorQueue);

		/** Prints the validation errors in the queue, in descending priority order */
		void printErrors(set<BoardInitializeError, ErrorPriorityFunction>* errorQueue);
	};
}