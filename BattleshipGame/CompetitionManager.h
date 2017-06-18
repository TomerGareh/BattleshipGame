#pragma once

#include <memory>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include "SingleGameTask.h"
#include "Scoreboard.h"
#include "AlgoLoader.h"
#include "BattleshipGameBoardFactory.h"

using std::vector;
using std::queue;
using std::shared_ptr;
using std::unique_ptr;
using std::thread;
using std::mutex;

namespace battleship
{
	/** Manages competition between all available player dlls on all available battleboards
	 */
	class CompetitionManager
	{
	public:
		/** Creates a new CompetitionManager which loads resources using the boardLoader and algoLoader.
		 *  threadCount is the amount of threads used to run games in parallel.
		 */
		CompetitionManager(shared_ptr<BattleshipGameBoardFactory> boardLoader,
						   shared_ptr<AlgoLoader> algoLoader,
						   int threadCount);
		virtual ~CompetitionManager() = default;

		/** Start digesting priority queue of games by worker threads and print round results when ready */
		void run();

		/** Logic for a single worker thread: constantly drain and process SingleGameTasks from gameSet until empty */
		void runWorkerThread(shared_ptr<BattleshipGameBoardFactory> boardLoader,
							 shared_ptr<AlgoLoader> algoLoader, int threadId);

	private:

		/** Priority queue of games in the competition, sorted by "game number" for each player so
		 *  matches are evenly distributed.
		 */
		queue<unique_ptr<SingleGameTask>> _gamesSet;

		/** Scoreboard of in game results for each round.
		 *  Functions relevant for competition time are protected by locks to enable concurrency.
		 */
		unique_ptr<Scoreboard> _scoreboard;

		/** Resources loader for available boards (creates new instances of BattleBoards) */
		shared_ptr<BattleshipGameBoardFactory> _boardLoader;

		/** Resources loader for available algorithms (creates new instances of IBattleShipGameAlgos) */
		shared_ptr<AlgoLoader> _algoLoader;

		/** List of worker threads that process the gameSet priority queue in parallel */
		vector<thread> _workerThreads;

		/** Locks the gameSet when multiple threads aim to pull from it */
		mutex _gameSetLock;

		/** Number of actual worker threads the competition manager employs */
		size_t _workerThreadsCount;

		/** Creates priority queue of games to run */
		void prepareCompetition(shared_ptr<BattleshipGameBoardFactory> boardLoader,
							    shared_ptr<AlgoLoader> algoLoader);
	};
}