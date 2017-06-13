#pragma once

#include <memory>
#include <vector>
#include <queue>
#include <atomic>
#include <thread>
#include <mutex>
#include "BattleBoard.h"
#include "SingleGameTask.h"
#include "Scoreboard.h"
#include "AlgoLoader.h"
#include "BattleshipGameBoardFactory.h"

using std::vector;
using std::priority_queue;
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

	private:
		/** Minimal space allocated for player name in the table (visual parameter) */
		static constexpr int MIN_PLAYER_NAME_SIZE = 12;

		/** Priority queue of games in the competition, sorted by "game number" for each player so
		 *  matches are evenly distributed.
		 */
		priority_queue<shared_ptr<SingleGameTask>> _gamesSet;

		/** Scoreboard of in game results for each round.
		 *  Functions relevant for competition time are protected by locks to enable concurrency.
		 */
		shared_ptr<Scoreboard> _scoreboard;

		/** Runs and manages a single game session.
		 *	This is a stateless object that can be used by multiple threads at once.
		 */
		GameManager _gameManager;

		/** Resources loader for available boards (creates new instances of BattleBoards) */
		shared_ptr<BattleshipGameBoardFactory> _boardLoader;

		/** Resources loader for available algorithms (creates new instances of IBattleShipGameAlgos) */
		shared_ptr<AlgoLoader> _algoLoader;

		/** List of worker threads that process the gameSet priority queue in parallel */
		vector<thread> _workerThreads;

		/** Locks the gameSet when multiple threads aim to pull from it */
		mutex _gameSetLock;

		/** Holds the longest player name encountered */
		int _maxPlayerNameLength;

		/** Creates priority queue of games to run */
		void prepareCompetition(shared_ptr<BattleshipGameBoardFactory> boardLoader,
							    shared_ptr<AlgoLoader> algoLoader);

		/** Logic for a single worker thread: constantly drain and process SingleGameTasks from gameSet until empty */
		void runWorkerThread(shared_ptr<BattleshipGameBoardFactory> boardLoader,
							 shared_ptr<AlgoLoader> algoLoader);

		/** Prints a formatted table of the round results to the console */
		void printRoundResults(shared_ptr<RoundResults> roundResults);

		/** Queries the RoundResults queue in the scoreboard, drains it and prints the stats to the console */
		void queryAndPrintScoreboard();
	};
}