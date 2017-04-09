#pragma once

#include <memory>
#include "BattleBoard.h"
#include "IBattleshipGameAlgo.h"
#include "IGameVisual.h"

using std::shared_ptr;
using std::unique_ptr;

namespace battleship
{
	const int FOREFEIT_COORDINATES = -1;

	class GameManager
	{
	public:
		GameManager();
		virtual ~GameManager();

		void startGame(shared_ptr<BattleBoard> board,
					   IBattleshipGameAlgo& playerA, IBattleshipGameAlgo& playerB,
					   IGameVisual& visualizer);

	private:
		bool isPlayerShipsLeft(BattleBoard* board, PlayerEnum player);
		bool isGameOver(BattleBoard* board, bool isPlayerAForfeit, bool isPlayerBForfeit);
		IBattleshipGameAlgo* switchPlayerTurns(IBattleshipGameAlgo& playerA, IBattleshipGameAlgo& playerB,
											   IBattleshipGameAlgo* currPlayer,
											   bool isPlayerAForfeit, bool isPlayerBForfeit);
	};
}
