#pragma once

#include <memory>
#include "IBattleshipGameAlgo.h"
#include "BattleBoard.h"

namespace battleship
{
	class GameFromFileAlgo : IBattleshipGameAlgo
	{
	public:
		GameFromFileAlgo();

		~GameFromFileAlgo();

		void setBoard(const char** board, int numRows, int numCols) override;

		std::pair<int, int> attack() override;

		void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;
	};
}