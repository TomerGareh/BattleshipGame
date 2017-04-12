#pragma once

#include "IGameVisual.h"
#include <memory>

using std::shared_ptr;

namespace battleship
{
	/** This module is responsible for generating simple output on the game results to the console.
	 *	This is the most minimal form of output the BattleShip game supports.
	 */
	class ConsoleMessageVisual : public IGameVisual
	{
	public:
		ConsoleMessageVisual();
		virtual ~ConsoleMessageVisual();

		virtual void visualizeBeginGame(shared_ptr<BattleBoard> board) override;
		virtual void visualizeAttackResults(shared_ptr<BattleBoard> board,
											int row, int col, AttackResult attackResults) override;
	};
}

