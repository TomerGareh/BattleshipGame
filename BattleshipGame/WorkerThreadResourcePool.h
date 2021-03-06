#pragma once

#include <memory>
#include <unordered_map>
#include "BattleshipGameBoardFactory.h"
#include "AlgoLoader.h"
#include "IBattleshipGameAlgo.h"
#include "BattleBoard.h"

using std::unique_ptr;
using std::shared_ptr;
using std::string;
using std::unordered_map;

namespace battleship
{
	/** Thread safe resource pool that caches loaded resources for each worker thread.
	 *  Cached resources are not shared among worker threads.
	 */
	class WorkerThreadResourcePool
	{
	public:
		WorkerThreadResourcePool(shared_ptr<BattleshipGameBoardFactory> boardLoader,
								 shared_ptr<AlgoLoader> algoLoader);
		virtual ~WorkerThreadResourcePool();

		/** Return algorithm in given path. Expected to be an algorithm that was loaded before.
		 *  If the algorithm exists in cache, it will be returned from the cache.
		 *  On error, nullptr is returned.
		 */
		IBattleshipGameAlgo* requestAlgo(const string& algoPath);

		/** Returns a new instance of the board in given path.
		 *  The board returned will be "clean" and ready for play.
		 *  Requested board are assumed to be loaded and valid.
		 *  On error, nullptr is returned.
		 */
		shared_ptr<BattleBoard> requestBoard(const string& boardPath) const;

		/** Resources cached by the player's algorithms are held here until
		 *  the users relinquish ownership over them. Since we can't trust the algorithm
		 *  not to access them after the game is over, we hold them here and manage their
		 *  lifetime until the algorithm have shifted to another game or have been destroyed.
		 */
		void cacheResourcesForPlayer(const string& player,
									 unique_ptr<BoardData> boardData);

	private:

		/** Loaders for boards and algorithms */
		shared_ptr<BattleshipGameBoardFactory> _boardLoader;
		shared_ptr<AlgoLoader> _algoLoader;

		/** Cache of loaded algos */
		unordered_map<string, unique_ptr<IBattleshipGameAlgo>> _algoPool;

		/** A map to keep an eye on player held resources resources.
		 *  Resources cached by the player's algorithms are held here until
		 *  the users relinquish ownership over them. Since we can't trust the algorithm
		 *  not to access them after the game is over, we hold them here and manage their
		 *  lifetime until the algorithm have shifted to another game or have been destroyed.
		 */
		unordered_map<string, unique_ptr<BoardData>> _playerHeldResources;
	};
}

