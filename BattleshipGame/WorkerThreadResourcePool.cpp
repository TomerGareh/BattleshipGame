#include "WorkerThreadResourcePool.h"

namespace battleship
{
	WorkerThreadResourcePool::WorkerThreadResourcePool(shared_ptr<BattleshipGameBoardFactory> boardLoader,
													   shared_ptr<AlgoLoader> algoLoader):
		_boardLoader(boardLoader),
		_algoLoader(algoLoader)
	{
	}

	WorkerThreadResourcePool::~WorkerThreadResourcePool()
	{
	}

	IBattleshipGameAlgo* WorkerThreadResourcePool::requestAlgo(const string& algoPath)
	{
		auto algoIt = _algoPool.find(algoPath);
		if (algoIt != _algoPool.end())
		{	// Exists in cache
			return algoIt->second.get();
		}
		else
		{	// Not loaded before, cache and return
			auto algo = _algoLoader->requestAlgo(algoPath);

			if (nullptr == algo)
				return nullptr;

			auto insertionPair = _algoPool.emplace(std::make_pair(algoPath, std::move(algo)));
			auto algoInstIt = insertionPair.first;
			return algoInstIt->second.get();
		}
	}

	shared_ptr<BattleBoard> WorkerThreadResourcePool::requestBoard(const string& boardPath) const
	{
		// Always request from board factory to create a new instance out of board prototype
		return _boardLoader->requestBattleboard(boardPath);
	}

	void WorkerThreadResourcePool::cacheResourcesForPlayer(const string& player,
														   unique_ptr<BoardData> boardData)
	{
		_playerHeldResources[player] = std::move(boardData);
	}
}