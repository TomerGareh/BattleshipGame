#include <iostream>
#include "GameVisual.h"

using std::cout;
using std::endl;

namespace battleship
{
	GameVisual::GameVisual()
	{
	}

	void GameVisual::visualizeBeginGame(shared_ptr<BattleBoard> board)
	{
		// Do nothing for Release
	}

	void GameVisual::visualizeEndGame(shared_ptr<BattleBoard> board, int playerAPoints, int playerBPoints)
	{
		bool isTie = ((board->getPlayerAShipCount() != 0) && (board->getPlayerBShipCount() != 0));
		if (!isTie)
		{
			bool isPlayerAWon = (board->getPlayerBShipCount() == 0);
			cout << "Player " << (isPlayerAWon ? "A" : "B") << " won" << endl;
		}

		cout << "Points:" << endl;
		cout << "Player A: " << playerAPoints << endl;
		cout << "Player B: " << playerBPoints << endl;
	}
}
