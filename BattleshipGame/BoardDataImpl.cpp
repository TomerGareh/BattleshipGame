#include "BoardDataImpl.h"
#include "BattleBoard.h"
#include <stdexcept>

using std::invalid_argument;

namespace battleship
{
	BoardDataImpl::BoardDataImpl(PlayerEnum player, shared_ptr<BattleBoard> board):
		BoardData(),
		_player(player),
		_board(board)
	{
		_rows = board->height();
		_cols = board->width();
		_depth = board->depth();
	}

	char BoardDataImpl::charAt(Coordinate c) const
	{
		auto gamePiece = _board->pieceAt(c);
		
		if (gamePiece == nullptr)
		{
			return static_cast<char>(BoardSquare::Empty); // Empty square
		}
		else if (gamePiece->_player != _player)
		{
			return static_cast<char>(BoardSquare::Empty); // Enemy piece
		}
		else
		{
			return static_cast<char>(gamePiece->_shipType->_representation); // Friendly ship
		}
	}
}