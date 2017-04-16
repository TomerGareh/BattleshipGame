#pragma once

#include "IBattleshipGameAlgo.h"
#include "BattleBoard.h"

namespace battleship
{
	/** An interface for all output visualizers that depict the gameplay output */
	class IGameVisual
	{
	public:
		IGameVisual();
		virtual ~IGameVisual() = default;

		/** Visualize the beginning of game state */
		virtual void visualizeBeginGame(shared_ptr<BattleBoard> board) = 0;

		/** Visualize the results of one of the player's attacks */
		virtual void visualizeAttackResults(shared_ptr<BattleBoard> board, int attackingPlayerNumber,
											int row, int col, AttackResult attackResults) = 0;

		/** Print the results of the end-game to the console (who won, how many points..) */
		virtual void visualizeEndGame(shared_ptr<BattleBoard> board,
									  int playerAWins, int playerBWins);
	};
}

