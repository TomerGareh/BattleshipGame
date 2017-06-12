#pragma once

#include "BattleBoard.h"

namespace battleship
{
	/** An interface for all output visualizers that depict the gameplay output */
	class GameVisual
	{
	public:
		GameVisual();
		virtual ~GameVisual() = default;

		/** Visualize the beginning of game state */
		void visualizeBeginGame(shared_ptr<BattleBoard> board);

		/** Print the results of the end-game to the console (who won, how many points..) */
		void visualizeEndGame(shared_ptr<BattleBoard> board, int playerAWins, int playerBWins);
	};
}

