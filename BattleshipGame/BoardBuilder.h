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
	/* Lower index is higher priority */
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

	class BoardBuilder
	{
	public:
		BoardBuilder();
		virtual ~BoardBuilder();

		class BoardInitializeError
		{
		public:
			BoardInitializeError(ErrorPriorityEnum errorType);

			~BoardInitializeError();
			
			const string& getMsg() const { return _msg; };
			
			const ErrorPriorityEnum getPriority() const { return _errorPriority; };
		private:
			string _msg;
			ErrorPriorityEnum _errorPriority;

			string getErrorMsg(ErrorPriorityEnum errorType);
		};

		// Typedefs
		using ErrorPriorityFunction = function<bool(const BoardInitializeError&, const BoardInitializeError&)>;

		BoardBuilder* addPiece(int row, int col, char type);

		shared_ptr<BattleBoard> build();

	private:
		class ShipMask
		{
		public:
			typedef list<tuple<int, int, char>> ShipMaskList;
			typedef unique_ptr<ShipMaskList> ShipMaskListPtr;
			
			ShipMaskListPtr mask;

			int matchSizeHorizontal;
			int matchSizeVertical;
			bool wrongSize;
			bool adjacentShips;

			ShipMask(BoardSquare ship);

			~ShipMask();

			bool applyMask(const shared_ptr<BattleBoard> board, int row, int col, PlayerEnum player);

			void resetMaskFlags();

			friend class BoardBuilder;
		};

		shared_ptr<BattleBoard> _board;

		void markVisitedSquares(bool visitedBoard[BOARD_SIZE][BOARD_SIZE], int row, int col, int size, Orientation orient);

		bool isValidBoard(set<BoardInitializeError, ErrorPriorityFunction>* errorQueue);

		void printErrors(set<BoardInitializeError, ErrorPriorityFunction>* errorQueue);

		bool validate();
	};
}