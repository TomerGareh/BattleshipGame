#pragma once

#include <memory>
#include "BattleBoard.h"
#include "IBattleshipGameAlgo.h"
#include "IGameVisual.h"

using std::shared_ptr;
using std::unique_ptr;

namespace battleship
{
	class GameManager
	{
	public:
		GameManager();
		virtual ~GameManager();

		void startGame(shared_ptr<BattleBoard> board,
					   IBattleshipGameAlgo& playerA, IBattleshipGameAlgo& playerB,
					   IGameVisual& visualizer);

	private:
		bool isPlayerShipsLeft(PlayerEnum player);
		bool isGameOver();
	};
}
