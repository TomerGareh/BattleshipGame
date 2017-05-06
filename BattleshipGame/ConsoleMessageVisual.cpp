#include "ConsoleMessageVisual.h"
#include "BattleBoard.h"

namespace battleship
{

	ConsoleMessageVisual::ConsoleMessageVisual() : IGameVisual()
	{
	}

	ConsoleMessageVisual::~ConsoleMessageVisual()
	{
	}

	void ConsoleMessageVisual::visualizeBeginGame(shared_ptr<BattleBoard> board)
	{
		// Console doesn't execute any setup logic in the beginning of the game
	}

	void ConsoleMessageVisual::visualizeAttackResults(shared_ptr<BattleBoard> board, int attackingPlayerNumber,
													  int row, int col, AttackResult attackResults,
											  		  shared_ptr<const GamePiece> attackedGamePiece)
	{
		// Console does not notify on attack results
	}

}