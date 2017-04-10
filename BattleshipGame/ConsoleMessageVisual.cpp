#include "ConsoleMessageVisual.h"
#include "BattleBoard.h"
#include <iostream>

using std::cout;
using std::endl;

namespace battleship
{

	ConsoleMessageVisual::ConsoleMessageVisual() : IGameVisual()
	{
	}

	ConsoleMessageVisual::~ConsoleMessageVisual()
	{
	}

	void ConsoleMessageVisual::visualizeAttackResults(int row, int col, AttackResult attackResults)
	{
		// Console does not notify on attack results
	}

	void ConsoleMessageVisual::visualizeEndGame(shared_ptr<BattleBoard> board,
												int playerAWins, int playerBWins)
	{
		bool isTie = ((board->getPlayerAShipCount() != 0) && (board->getPlayerBShipCount() != 0));
		if (!isTie)
		{
			bool isPlayerAWon = (board->getPlayerBShipCount() == 0);
			cout << "Player " << (isPlayerAWon ? "A" : "B") << " won" << endl;
		}
		
		cout << "Points:" << endl;
		cout << "Player A: " << playerAWins << endl;
		cout << "Player B: " << playerBWins << endl;
	}
}