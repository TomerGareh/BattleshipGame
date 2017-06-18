#include "CompetitionManager.h"
#include "Logger.h"
#include <string>
#include <algorithm>

using std::lock_guard;
using std::to_string;
using std::min;
using std::max;

namespace battleship
{
	void CompetitionManager::prepareCompetition(shared_ptr<BattleshipGameBoardFactory> boardLoader,
												shared_ptr<AlgoLoader> algoLoader)
	{
		auto boards = boardLoader->loadedBoardsList(); // Valid boards
		auto algos = algoLoader->loadedGameAlgos(); // Valid loaded algorithms

		// Total games for each player: play twice against each player other player on each board
		auto totalRounds = (algos.size() - 1) * 2 * boards.size();

		// Reset scoreboard (casting totalRounds to int is safe since we don't expect that many games)
		_scoreboard = std::make_unique<Scoreboard>(algos, static_cast<int>(totalRounds));
		
		// Iterate all boards and players and create SingleGameTask for each valid combination
		queue<unique_ptr<SingleGameTask>> inversedGamesSet;
		size_t numOfAlgos = algos.size() - 1;
		for (const auto& board : boards)
		{
			// Each round is a diagonal in the game matrix (without the main diagonal),
			// and we run over it from both sides in order to get a balanced tournament 
			for (size_t round = 1; round <= numOfAlgos; ++round)
			{
				for (size_t algo1 = 0, algo2 = round; ((algo1 <= (numOfAlgos - algo2)) && (algo2 <= numOfAlgos)); ++algo1, ++algo2)
				{
					_gamesSet.push(std::make_unique<SingleGameTask>(algos[algo1], algos[algo2], board));
					inversedGamesSet.push(std::make_unique<SingleGameTask>(algos[algo2], algos[algo1], board));
					if (algo1 != (numOfAlgos - algo2))	// On the secondary diagonal 'algo1' and 'numOfAlgos-algo2' indices meet
														// and we don't want to add them twice
					{
						_gamesSet.push(std::make_unique<SingleGameTask>(algos[numOfAlgos-algo2], algos[numOfAlgos-algo1], board));
						inversedGamesSet.push(std::make_unique<SingleGameTask>(algos[numOfAlgos-algo1], algos[numOfAlgos-algo2], board));
					}
				}
			}
		}

		// Move inversed games to the main queue
		while (!inversedGamesSet.empty())
		{
			_gamesSet.push(std::move(inversedGamesSet.front()));
			inversedGamesSet.pop();
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
			unique_ptr<SingleGameTask> task;

			// Protect the game-set queue from concurrent access, each worker fetches a task and releases the lock
			{
				lock_guard<mutex> lock(_gameSetLock);
				if (_gamesSet.empty()) // Check that nobody stole the last game before the snatched the lock
					break;

				// Pop next game task from game-queue.
				// Games are expected to be pre-sorted in a fair manner for all players.
				task = std::move(_gamesSet.front());
				_gamesSet.pop();
			}

			// Lock is released and the thread now runs the game the task represents
			if (task != nullptr)
			{
				task->run(resourcePool, _scoreboard.get());
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
			if (!_gamesSet.empty())
			{
				_workerThreads.push_back(std::move(thread(&CompetitionManager::runWorkerThread,
										 this, _boardLoader, _algoLoader, threadId)));
			}
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