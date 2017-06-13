#include "CompetitionManager.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

using std::lock_guard;
using std::setw;
using std::setprecision;
using std::cout;
using std::endl;
using std::min;

namespace battleship
{
	void CompetitionManager::prepareCompetition(shared_ptr<BattleshipGameBoardFactory> boardLoader,
												shared_ptr<AlgoLoader> algoLoader)
	{
		auto availableBoards = boardLoader->boardsList(); // Valid boards
		auto availableAlgos = algoLoader->loadedGameAlgos(); // Valid loaded algorithms
		_scoreboard = std::make_shared<Scoreboard>(availableAlgos); // Reset scoreboard

		// Iterate all boards and players and create SingleGameTask for each valid combination
		for (const auto& board : availableBoards)
		{
			for (const auto& algo1 : availableAlgos)
			{
				for (const auto& algo2 : availableAlgos)
				{
					if (algo1 != algo2) // Avoid games with the same player against himself
					{
						// _gameSet keeps all SingleGameTasks sorted in a fair order for the players
						// so everyone gets their chance to play equally
						_gamesSet.emplace(std::make_shared<SingleGameTask>(algo1, algo2,
																		   board, _scoreboard.get()));
					}
				}
			}
		}
	}

	CompetitionManager::CompetitionManager(shared_ptr<BattleshipGameBoardFactory> boardLoader,
										   shared_ptr<AlgoLoader> algoLoader,
										   int threadCount):
										   _boardLoader(boardLoader),
										   _algoLoader(algoLoader)
	{
		// Fill priority queue with tasks for all possible games in competition
		prepareCompetition(boardLoader, algoLoader);

		// Don't use more threads than needed, even if count says so
		auto actualThreadCount = (threadCount < _gamesSet.size()) ? threadCount : _gamesSet.size();
		_workerThreads.reserve(actualThreadCount);

		// Save max player name for score results table formatting
		_maxPlayerNameLength = MIN_PLAYER_NAME_SIZE;
		for (const auto& algoName : algoLoader->loadedGameAlgos())
		{
			if (_maxPlayerNameLength < algoName.length())
				_maxPlayerNameLength = algoName.length();
		}
	}

	void CompetitionManager::runWorkerThread(shared_ptr<BattleshipGameBoardFactory> boardLoader,
											 shared_ptr<AlgoLoader> algoLoader)
	{
		// Each thread keeps it's own pool of resources that are created on demand,
		// to avoid wasting time on locking shared resources between multiple threads
		WorkerThreadResourcePool resourcePool(boardLoader, algoLoader);

		while (!_gamesSet.empty()) // While there are still games to be played
		{
			shared_ptr<SingleGameTask> task;

			// Protect the game-set queue from concurrent access, each worker fetches a task and releases the lock
			{
				lock_guard<mutex> lock(_gameSetLock);
				if (_gamesSet.empty()) // Check that nobody stole the last game before the snatched the lock
					break;

				// Pop next game task from game-queue.
				// Games are expected to be pre-sorted in a fair manner for all players.
				task = _gamesSet.top();
				_gamesSet.pop();
			}

			// Lock is released and the thread now runs the game the task represents
			if (task != NULL)
			{
				task->run(_gameManager, resourcePool);
			}
		}
	}

	void CompetitionManager::printRoundResults(shared_ptr<RoundResults> roundResults)
	{
		cout << setw(8) << "#"
			 << setw(_maxPlayerNameLength) << "Team Name"
			 << setw(8) << "Wins"
			 << setw(8) << "Losses"
			 << setw(8) << "%"
			 << setw(8) << "Pts For"
			 << setw(12) << "Pts Against" << endl;

		int place = 1;

		// RoundsResults are already sorted by player's rating
		for (const auto& playerStats : roundResults->playerStatistics)
		{
			string placeStr = place + ".";
			cout << setw(8) << placeStr
				 << setw(_maxPlayerNameLength) << playerStats.playerName
				 << setw(8) << playerStats.wins
				 << setw(8) << playerStats.loses
				 << setw(8) << setprecision(2) << playerStats.rating
				 << setw(8) << playerStats.pointsFor
				 << setw(12) << playerStats.pointsAgainst << endl;
			place++;
		}
	}

	void CompetitionManager::queryAndPrintScoreboard()
	{
		// Print all ready round results
		// After they are printed, we pop this data from the queue
		while (!_scoreboard->getRoundResults().empty)
		{
			auto nextResults = _scoreboard->getRoundResults().front();
			_scoreboard->getRoundResults().pop_back();
			printRoundResults(nextResults);
		}
	}

	void CompetitionManager::run()
	{
		// Start all worker threads
		for (auto& worker: _workerThreads)
		{
			worker = thread(&CompetitionManager::runWorkerThread, this, _boardLoader, _algoLoader);
		}

		// Print all ready round results from the scoreboard and drain the RoundResults queue
		auto roundResultsReadyCallback = &queryAndPrintScoreboard;

		// While competition is not over, wake up when round results are ready and print them
		while (!_gamesSet.empty())
		{
			// Wait on conditional_variable predicate and wake up when round results are ready
			// Then trigger callback which prints the results
			_scoreboard->waitOnRoundResults(roundResultsReadyCallback);
		}

		// Wait for all worker threads to finish
		for (auto& worker : _workerThreads)
		{
			if (worker.joinable())
			{
				worker.join();
			}
		}

		// Print the last results that may have arrived after all worker threads have finished
		queryAndPrintScoreboard();
	}
}