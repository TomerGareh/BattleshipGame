#include "BoardDataImpl.h"
#include <stdexcept>

using std::invalid_argument;

namespace battleship
{
	BoardDataImpl::BoardDataImpl(PlayerEnum player, GamePiecesDict* gamePieces,
								 int boardRows, int boardCols, int boardDepth): BoardData()
	{
		_rows = boardRows;
		_cols = boardCols;
		_depth = boardDepth;

		// Iterate all game pieces
		for (auto const &pieceEntry : *gamePieces)
		{
			auto gamePiece = pieceEntry.second;

			// Add only the player's ships (sparse representation)
			if (gamePiece->_player == player)
			{
				char shipSymbol = static_cast<char>(gamePiece->_shipType->_representation);
				_visiblePieces.emplace(pieceEntry.first, // Coordinate
									   shipSymbol);
			}
		}
	}

	char BoardDataImpl::charAt(Coordinate c) const
	{
		auto gamePieceIt = _visiblePieces.find(c);
		
		// If not entry exists return EMPTY
		if (gamePieceIt == _visiblePieces.end())
		{
			return static_cast<char>(BoardSquare::Empty);
		}
		else
		{ // If an entry exists return the game piece
			return gamePieceIt->second;
		}
	}
}