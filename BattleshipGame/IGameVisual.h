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

		virtual void visualizeBeginGame(shared_ptr<BattleBoard> board) = 0;
		virtual void visualizeAttackResults(shared_ptr<BattleBoard> board,
											int row, int col, AttackResult attackResults) = 0;
		virtual void visualizeEndGame(shared_ptr<BattleBoard> board,
									  int playerAWins, int playerBWins);
	};
}

