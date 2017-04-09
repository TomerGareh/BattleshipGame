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

		virtual void visualizeAttackResults(int row, int col, AttackResult attackResults) override;
		virtual void visualizeEndGame(shared_ptr<BattleBoard> board,
									  int playerAWins, int playerBWins) override;
	};
}

