#include <iostream>
#include "GameManager.h"
#include "AlgoCommon.h"

using std::cout;
using std::endl;

namespace battleship
{
	GameManager::GameManager()
	{
	}

	shared_ptr<IBattleshipGameAlgo> GameManager::initPlayer(int playerNum,
														    AlgoLoader& algoLoader,
														    shared_ptr<BattleBoard> board,
														    const string& resourcesPath)
	{
		// Step #1: Load algorithm's DLL
		shared_ptr<IBattleshipGameAlgo> player = algoLoader.loadAlgoByLexicalOrder(playerNum);

		// Avoid DLL load errors
		if (NULL == player)
			return NULL;
		
		player->setBoard(playerNum,	board->getBoardPlayerView(static_cast<PlayerEnum>(playerNum)).get(),
						 BOARD_SIZE, BOARD_SIZE);

		// Step #2: Initialize the algorithm
		bool initSuccess = player->init(resourcesPath);
		if (!initSuccess)
		{ // Algorithm failed to initialize
			const string algoPath = algoLoader.getAlgoPathByIndex(playerNum);
			cout << "Algorithm initialization failed for dll: " << algoPath << endl;
			return NULL;
		}

		return player;
	}

	bool GameManager::isPlayerShipsLeft(const BattleBoard *const board, PlayerEnum player) const
	{
		const int shipsCount = (player == PlayerEnum::A) ?
								board->getPlayerAShipCount() : board->getPlayerBShipCount();
		return (shipsCount > 0);
	}

	bool GameManager::isGameOver(const BattleBoard *const board, bool isPlayerAForfeit, bool isPlayerBForfeit) const
	{
		if ((isPlayerAForfeit && isPlayerBForfeit) ||
			(!isPlayerShipsLeft(board, PlayerEnum::A)) ||
			(!isPlayerShipsLeft(board, PlayerEnum::B)))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	IBattleshipGameAlgo* GameManager::switchPlayerTurns(IBattleshipGameAlgo& playerA,
														IBattleshipGameAlgo& playerB,
														IBattleshipGameAlgo* currPlayer,
														shared_ptr<const GamePiece> lastAttackedPiece,
														bool isPlayerAForfeit, bool isPlayerBForfeit) const
	{
		bool isCurrPlayerA = (currPlayer == &playerA);
		bool isCurrPlayerB = !isCurrPlayerA;

		// Switch turns only if the player misses or hits himself
		if ((lastAttackedPiece == NULL) ||
			((lastAttackedPiece->_player == PlayerEnum::A) && (isCurrPlayerA)) ||
			((lastAttackedPiece->_player == PlayerEnum::B) && (isCurrPlayerB)))
		{
			if (isCurrPlayerA && (!isPlayerBForfeit)) // A just played and B still didn't forfeit
				return &playerB;
			else if (!isPlayerAForfeit)	// B forfeited or B just played now
				return &playerA;
			else
				return &playerB;
		}
		else
		{
			return currPlayer;
		}
	}

	void GameManager::updateCurrentGamePoints(const GamePiece *const sankPiece,
											  int& playerAScore, int& playerBScore) const
	{
		if (sankPiece->_player == PlayerEnum::A)
		{
			playerBScore += sankPiece->_shipType->_points;
		}
		else
		{
			playerAScore += sankPiece->_shipType->_points;
		}
	}

	void GameManager::updateScoreboard(const BattleBoard *const board)
	{
		if (board->getPlayerAShipCount() == 0)
		{
			_playerBWins++;
		}
		else if (board->getPlayerBShipCount() == 0)
		{
			_playerAWins++;
		}

		// Otherwise - this is a tie
	}

	void GameManager::startGame(shared_ptr<BattleBoard> board,
								shared_ptr<IBattleshipGameAlgo> playerA, shared_ptr<IBattleshipGameAlgo> playerB,
								IGameVisual& visualizer)
	{
		visualizer.visualizeBeginGame(board);

		IBattleshipGameAlgo* currentPlayer = playerA.get();
		bool isPlayerAForfeit = false;
		bool isPlayerBForfeit = false;
		int playerAPoints = 0;
		int playerBPoints = 0;

		while (!isGameOver(board.get(), isPlayerAForfeit, isPlayerBForfeit))
		{
			// Attack
			auto target = currentPlayer->attack();

			if (target == NO_MORE_MOVES)
			{	// Player chose not to attack - from now on this player forfeits the game
				if (currentPlayer == playerA.get())
					isPlayerAForfeit = true;
				else
					isPlayerBForfeit = true;

				currentPlayer = switchPlayerTurns(*playerA, *playerB, currentPlayer, NULL,
												  isPlayerAForfeit, isPlayerBForfeit);
				continue;
			}
			else if ((target.first < 1) || (target.second < 1) ||
					 (target.first > BOARD_SIZE) || (target.second > BOARD_SIZE))
			{
				// Play performed an illegal move and will lose his turn
				currentPlayer = switchPlayerTurns(*playerA, *playerB, currentPlayer, NULL,
												  isPlayerAForfeit, isPlayerBForfeit);

				continue;
			}

			// Normalize coordinates to 0~BOARD_SIZE-1
			target.first--;
			target.second--;

			// Execute attack move on the board itself and update the game-pieces status
			// We get in return an object that describes the result of the attack
			auto attackedGamePiece = board->executeAttack(target);

			// Notify on attack results
			int attackingPlayerNumber = (currentPlayer == playerB.get()); // A - 0, B - 1
			AttackResult attackResult;

			if (attackedGamePiece == NULL)
			{	// Miss
				attackResult = AttackResult::Miss;
			}
			else if (attackedGamePiece->_lifeLeft == 0)
			{	// Sink
				attackResult = AttackResult::Sink;
				updateCurrentGamePoints(attackedGamePiece.get(), playerAPoints, playerBPoints);
			}
			else
			{	// Hit
				attackResult = AttackResult::Hit;
			}

			currentPlayer = switchPlayerTurns(*playerA, *playerB, currentPlayer, attackedGamePiece,
											  isPlayerAForfeit, isPlayerBForfeit);

			playerA->notifyOnAttackResult(attackingPlayerNumber, target.first + 1, target.second + 1, attackResult);
			playerB->notifyOnAttackResult(attackingPlayerNumber, target.first + 1, target.second + 1, attackResult);
			visualizer.visualizeAttackResults(board, attackingPlayerNumber,
											  target.first, target.second, attackResult, attackedGamePiece);
		}

		updateScoreboard(board.get());
		visualizer.visualizeEndGame(board, playerAPoints, playerBPoints);
	}
}