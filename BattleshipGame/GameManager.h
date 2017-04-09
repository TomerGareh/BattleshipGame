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
		int _playerAWins = 0;
		int _playerBWins = 0;

		bool isPlayerShipsLeft(BattleBoard* board, PlayerEnum player) const;
		bool isGameOver(BattleBoard* board, bool isPlayerAForfeit, bool isPlayerBForfeit) const;
		IBattleshipGameAlgo* switchPlayerTurns(IBattleshipGameAlgo& playerA, IBattleshipGameAlgo& playerB,
											   IBattleshipGameAlgo* currPlayer,
											   bool isPlayerAForfeit, bool isPlayerBForfeit) const;
		void updateCurrentGamePoints(const GamePiece *const sankPiece, int& playerAScore, int& playerBScore) const;
		void updateScoreboard(const BattleBoard *const board);
	};
}
