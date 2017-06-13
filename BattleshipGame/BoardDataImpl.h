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
	class BattleBoard;

	/** A player's view of the given wrapped battleboard.
	 *	The inner battleboard is immutable to this class.
	 */
	class BoardDataImpl : public BoardData
	{
	public:
		/** Construct view of the board from given player point of view */
		BoardDataImpl(PlayerEnum player, shared_ptr<BattleBoard> board);
		virtual ~BoardDataImpl() = default;

		// Returns only selected players' chars
		virtual char charAt(Coordinate c) const override;

	private:
		PlayerEnum _player;
		shared_ptr<BattleBoard> _board;
	};
}