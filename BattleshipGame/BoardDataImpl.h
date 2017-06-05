#pragma once
#include "IBattleshipGameAlgo.h"
#include "AlgoCommon.h"
#include "BattleBoard.h"
#include <tuple>
#include <unordered_map>

using std::unordered_map;
using std::tuple;

namespace battleship
{
	class BoardDataImpl : public BoardData
	{
	public:
		BoardDataImpl(PlayerEnum player, GamePiecesDict* gamePieces,
					  int rows, int cols, int depth);

		virtual ~BoardDataImpl() = default;

		virtual char charAt(Coordinate c) const override; //returns only selected players' chars

		int rows()  const { return _rows; }
		int cols()  const { return _cols; }
		int depth() const { return _depth; }

	private:
		unordered_map<Coordinate, char> _visiblePieces;
	};
}