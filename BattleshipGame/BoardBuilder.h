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
		WRONG_SIZE_SHAPE_FOR_SHIP_PLAYER_A = 0,
		WRONG_SIZE_SHAPE_FOR_SHIP_PLAYER_B = 1,
		TOO_MANY_SHIPS_PLAYER_A = 2,
		TOO_FEW_SHIPS_PLAYER_A = 3,
		TOO_MANY_SHIPS_PLAYER_B = 4,
		TOO_FEW_SHIPS_PLAYER_B = 5,
		ADJACENT_SHIPS_ON_BOARD = 6
	};

	class BoardBuilder
	{
	public:
		BoardBuilder();
		virtual ~BoardBuilder();

		class BoardInitializeError
		{
		public:
			const string& getMsg() const { return _msg; };
			const ErrorPriorityEnum getPriority() const { return _errorPriority; };
		private:
			string _msg;
			battleship::ErrorPriorityEnum _errorPriority;

			string getErrorMsg(battleship::ErrorPriorityEnum errorType)
			{
				// TODO:
				/*
					Wrong size or shape for ship <char> for player A
					Wrong size or shape for ship <char> for player B
					Too many ships for player A
					Too few ships for player A
					Too many ships for player B
					Too few ships for player B
					Adjacent Ships on Board
				*/
			}
		};

		// Typedefs
		using ErrorPriorityFunction = function<bool(const BoardInitializeError&, const BoardInitializeError&)>;

		BoardBuilder* addPiece(int row, int col, char type);

		shared_ptr<BattleBoard> build();

	private:
		class ShipMask
		{
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

			void resetMatchSizes();

			friend class BoardBuilder;
		};

		shared_ptr<BattleBoard> _board;

		void markVisitedSquares(bool visitedBoard[BOARD_SIZE][BOARD_SIZE], int row, int col, int size, Orientation orient);

		bool isValidBoard();

		void printErrors();

		bool validate();
	};
}