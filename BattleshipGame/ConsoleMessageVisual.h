#pragma once

#include "IGameVisual.h"
#include <memory>

using std::shared_ptr;

namespace battleship
{
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

