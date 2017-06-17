#include "CompetitionManager.h"
#include "Logger.h"
#include <string>
#include <algorithm>

using std::priority_queue;
using std::lock_guard;
using std::to_string;
using std::min;
using std::max;

namespace battleship
{
	void CompetitionManager::prepareCompetition(shared_ptr<BattleshipGameBoardFactory> boardLoader,
												shared_ptr<AlgoLoader> algoLoader)
	{
		auto availableBoards = boardLoader->boardsList(); // Valid boards
		auto availableAlgos = algoLoader->loadedGameAlgos(); // Valid loaded algorithms

		// Total games for each player: play twice against each player other player on each board
		auto totalRounds = (availableAlgos.size() - 1) * 2 * availableBoards.size();

		// Reset scoreboard (casting totalRounds to int is safe since we don't expect that many games)
		_scoreboard = std::make_shared<Scoreboard>(availableAlgos, static_cast<int>(totalRounds));

		unordered_map<string, int> registeredGames;
		for (const auto& algo : availableAlgos)
		{
			registeredGames.emplace(algo, 0);
		}

		// For reordering SingleGameTask by player's game time.
		// Helps sorting SingleGameTasks in the priority queue to keep the competition fair for all player's
		// chance of playing.
		auto PrioritySort = [this, &registeredGames](const shared_ptr<SingleGameTask> lhs,
													 const shared_ptr<SingleGameTask> rhs) -> bool
		{
			auto lhsPlayerAGamesCount = registeredGames.find(lhs->playerAName())->second;
			auto lhsPlayerBGamesCount = registeredGames.find(lhs->playerBName())->second;
			auto rhsPlayerAGamesCount = registeredGames.find(rhs->playerAName())->second;
			auto rhsPlayerBGamesCount = registeredGames.find(rhs->playerBName())->second;

			// Game with the player that played the LEAST amount of games wins HIGHER priority
			// (Note: Intuitively, this is in inverse order to played amound of games!)
			int lowestLhsPlayer = min(lhsPlayerAGamesCount, lhsPlayerBGamesCount);
			int lowestRhsPlayer = min(rhsPlayerAGamesCount, rhsPlayerBGamesCount);
			if (lowestLhsPlayer != lowestRhsPlayer)
			{
				return lowestLhsPlayer > lowestRhsPlayer;
			}
			
			int highestLhsPlayer = max(lhsPlayerAGamesCount, lhsPlayerBGamesCount);
			int highestRhsPlayer = max(rhsPlayerAGamesCount, rhsPlayerBGamesCount);
			if (highestLhsPlayer < highestRhsPlayer)
			{
				return highestLhsPlayer > highestRhsPlayer;
			}

			// Else sort lexicographically so set won't dump "equal" items for the set
			if (lhs->playerAName() != rhs->playerAName())
			{
				return lhs->playerAName() > rhs->playerAName();
			}
			else if (lhs->playerBName() != rhs->playerBName())
			{
				return lhs->playerBName() > rhs->playerBName();
			}
			else
			{
				return lhs->boardName() > rhs->boardName();
			}
		};

		set<shared_ptr<SingleGameTask>, decltype(PrioritySort)> gamesToSortQueue(PrioritySort);

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
						gamesToSortQueue.emplace(std::make_shared<SingleGameTask>(algo1, algo2,
																				  board, _scoreboard.get()));
					}
				}
			}
		}

		while (!gamesToSortQueue.empty())
		{
			// Get next game in fair order
			auto task = *gamesToSortQueue.begin();

			gamesToSortQueue.erase(gamesToSortQueue.begin());
			_gamesSet.push(task);

			// Re-prioritize queue - no need to check for end here since we've just created all keys
			registeredGames.find(task->playerAName())->second++;
			registeredGames.find(task->playerBName())->second++;
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
		_workerThreadsCount = threadCount < static_cast<int>(_gamesSet.size()) ? 
							  threadCount : static_cast<int>(_gamesSet.size());
		_workerThreads.reserve(_workerThreadsCount);
	}

	void CompetitionManager::runWorkerThread(shared_ptr<BattleshipGameBoardFactory> boardLoader,
											 shared_ptr<AlgoLoader> algoLoader,
											 int threadId)
	{
		// Each thread keeps it's own pool of resources that are created on demand,
		// to avoid wasting time on locking shared resources between multiple threads
		WorkerThreadResourcePool resourcePool(boardLoader, algoLoader);

		Logger::getInstance().log(Severity::INFO_LEVEL, "Worker thread #" + to_string(threadId) + " started..");

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
				task = _gamesSet.front();
				_gamesSet.pop();
			}

			// Lock is released and the thread now runs the game the task represents
			if (task != nullptr)
			{
				task->run(_gameManager, resourcePool);
			}
		}

		Logger::getInstance().log(Severity::INFO_LEVEL, "Worker thread #" + to_string(threadId) + " finished..");
	}

	void CompetitionManager::run()
	{
		if (_workerThreadsCount < 1)
		{
			Logger::getInstance().log(Severity::ERROR_LEVEL,
								 	  "Attempted to start competition with illegal number of worker threads: " +
									  to_string(static_cast<unsigned int>(_workerThreadsCount)));
			return;
		}

		Logger::getInstance().log(Severity::INFO_LEVEL,
								  "Competition started with " + 
								  to_string(static_cast<unsigned int>(_gamesSet.size())) +
			                      " games run by " +
								  to_string(static_cast<unsigned int>(_workerThreadsCount)) +
								  " threads.");

		// Start all worker threads
		for (int threadId = 1; threadId <= _workerThreadsCount; threadId++)
		{
			_workerThreads.push_back(std::move(thread(&CompetitionManager::runWorkerThread, 
									 this, _boardLoader, _algoLoader, threadId)));
			threadId++;
		}

		// While competition is not over, wake up when round results are ready and print them
		while (!_gamesSet.empty())
		{
			// Wait on conditional_variable predicate and wake up when some round results are ready
			// Then print all ready round results from the scoreboard and drain the RoundResults queue
			_scoreboard->waitOnRoundResults();
		}

		// Wait for all worker threads to finish
		for (auto& worker : _workerThreads)
		{
			if (worker.joinable())
			{
				worker.join();
			}
		}

		// Drain any remaining round results in queue and report to screen / log
		_scoreboard->processRoundResultsQueue();
	}
}