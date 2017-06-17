#include "CompetitionManager.h"
#include "Logger.h"
#include <string>

using std::lock_guard;
using std::to_string;

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
				task = _gamesSet.top();
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
		Logger::getInstance().log(Severity::INFO_LEVEL,
								  "Competition started with " + 
								  to_string(static_cast<unsigned int>(_gamesSet.size())) +
			                      " games ran by " +
								  to_string(static_cast<unsigned int>(_workerThreads.size())) +
								  " threads.");

		int threadId = 1;

		// Start all worker threads
		for (auto& worker: _workerThreads)
		{
			worker = thread(&CompetitionManager::runWorkerThread, this, _boardLoader, _algoLoader, threadId);
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
	}
}