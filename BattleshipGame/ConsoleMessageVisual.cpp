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
		// TODO: What should we do here?
	}

	void ConsoleMessageVisual::visualizeEndGame(shared_ptr<BattleBoard> board,
												bool isPlayerAForfeit, bool isPlayerBForfeit)
	{
		bool isPlayerAWon = (board->getPlayerBShipCount == 0);
		if (!(isPlayerAForfeit && isPlayerBForfeit))
		{
			bool isPlayerAWon = (board->getPlayerBShipCount == 0);
			cout << "Player " << (isPlayerAWon ? "A" : "B") << " won" << endl;
		}
		
		cout << "Points:" << endl;
		cout << "Player A: " << "to be implemented. " << endl;
		cout << "Player B: " << "to be implemented. " << endl;
	}
}