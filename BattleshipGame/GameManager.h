#pragma once

#include <memory>
#include "BattleBoard.h"
#include "IBattleshipGameAlgo.h"

using std::shared_ptr;
using std::unique_ptr;

namespace battleship
{
	/** Special attack coordinates returned when the player chooses not to attack but to forfeit the game. */
	const int FOREFEIT_COORDINATES = -1;

	/** Contains the game results of a single session */
	struct GameResults
	{
		PlayerEnum winner;
		int playerAPoints;
		int playerBPoints;
	};

	/** Manages a session of a single game, in stateless manner to enable thread-saftey */
	class GameManager
	{
	public:
		GameManager();
		virtual ~GameManager() = default;

		/** Starts a new game session using the given board, between the 2 players algorithms.
		 */
		shared_ptr<GameResults> runGame(shared_ptr<BattleBoard> board,
										shared_ptr<IBattleshipGameAlgo> playerA,
										shared_ptr<IBattleshipGameAlgo> playerB,
										const BoardData& playerAView,
										const BoardData& playerBView) const;

	private:

		/** Helper methods: Are all game pieces of player X gone? */
		static bool isPlayerShipsLeft(const BattleBoard *const board, PlayerEnum player);

		/** Returns if both player forfeit or anybody have won */
		static bool isGameOver(const BattleBoard* board, bool isPlayerAForfeit, bool isPlayerBForfeit);

		/** Takes into consideration the current game state and points currPlayer to the next player who
		 *  should be playing
		 */
		static IBattleshipGameAlgo* switchPlayerTurns(IBattleshipGameAlgo& playerA, IBattleshipGameAlgo& playerB,
											   IBattleshipGameAlgo* currPlayer,
											   shared_ptr<const GamePiece> lastAttackedPiece,
											   bool isPlayerAForfeit, bool isPlayerBForfeit);

		/** Updates the game points when a game piece have been sank */
		static void updateCurrentGamePoints(const GamePiece *const sankPiece, int& playerAScore, int& playerBScore);

		/** Fetch the winning player from the board according to the final game state */
		static PlayerEnum GameManager::getWinner(const BattleBoard *const board);
	};
}
