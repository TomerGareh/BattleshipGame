#pragma once

#include "IBattleshipGameAlgo.h"
#include "BattleBoard.h"

namespace battleship
{
	class IGameVisual
	{
	public:
		IGameVisual();
		virtual ~IGameVisual();

		virtual void visualizeAttackResults(int row, int col, AttackResult attackResults) = 0;
		virtual void visualizeEndGame(shared_ptr<BattleBoard> board,
									  bool isPlayerAForfeit, bool isPlayerBForfeit) = 0;
	};
}

