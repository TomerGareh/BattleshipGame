#include "IGameVisual.h"
#include <iostream>

using std::cout;
using std::endl;

namespace battleship
{
	IGameVisual::IGameVisual()
	{
	}

	void IGameVisual::visualizeEndGame(shared_ptr<BattleBoard> board, int playerAPoints, int playerBPoints)
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
