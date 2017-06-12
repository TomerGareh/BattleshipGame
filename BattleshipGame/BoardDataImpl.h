#pragma once

#include <unordered_map>
#include "IBattleshipGameAlgo.h"
#include "BattleBoard.h"
#include "AlgoCommon.h"

using std::unordered_map;

namespace battleship
{
	class BoardDataImpl : public BoardData
	{
	public:
		BoardDataImpl(PlayerEnum player, const GamePiecesDict* gamePieces,
					  int rows, int cols, int depth);

		virtual ~BoardDataImpl() = default;

		virtual char charAt(Coordinate c) const override; //returns only selected players' chars

		int rows()  const { return _rows; }
		int cols()  const { return _cols; }
		int depth() const { return _depth; }

	private:
		unordered_map<Coordinate, char, CoordinateHash> _visiblePieces;
	};
}