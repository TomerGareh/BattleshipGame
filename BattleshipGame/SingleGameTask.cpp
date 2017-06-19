#include "SingleGameTask.h"
#include "BoardDataImpl.h"
#include "Logger.h"
#include <algorithm>

using std::min;
using std::max;

namespace battleship
{
	SingleGameTask::SingleGameTask(const string& playerAName, const string& playerBName,
								   const string& boardName):
		_playerAName(playerAName),
		_playerBName(playerBName),
		_boardName(boardName)
	{
		string msg = "Created game between Player A: " + _playerAName +
					 " and Player B: " + _playerBName +
					 " on board: " + _boardName + ".";
		Logger::getInstance().log(Severity::DEBUG_LEVEL, msg);
	}

	void SingleGameTask::run(WorkerThreadResourcePool& resourcePool, Scoreboard* scoreBoard) const
	{
		// Load resources
		auto playerA = resourcePool.requestAlgo(_playerAName);
		auto playerB = resourcePool.requestAlgo(_playerBName);
		auto board = resourcePool.requestBoard(_boardName);

		if ((playerA == nullptr) || (playerB == nullptr) || (board == nullptr))
		{
			string msg = "Error: Can't start a game between Player A: " + _playerAName +
						 " and Player B: " + _playerBName +
					     " on board: " + _boardName + " due to invalid resources";
			Logger::getInstance().log(Severity::ERROR_LEVEL, msg);

			// Declare a tie so we won't be missing games for a round
			GameResults gameResults{ PlayerEnum::NONE, 0, 0 };
			scoreBoard->updateWithGameResults(gameResults, _playerAName, _playerBName, _boardName);
			return;
		}

		Logger::getInstance().log(Severity::DEBUG_LEVEL,
								  "Game started between Player A: " + _playerAName +
								  " and Player B: " + _playerBName + " on board: " + _boardName + ".");

		// Player views will be kept alive for the duration of the game (this scope)
		auto playerAView = std::make_unique<BoardDataImpl>(PlayerEnum::A, board);
		auto playerBView = std::make_unique<BoardDataImpl>(PlayerEnum::B, board);

		// Run a single game and update scoreboard with results
		auto gameResults = GameManager::runGame(board, playerA, playerB, *playerAView, *playerBView);

		// Keep player held views alive until the player gets a new board from the next game.
		// This should prevent pesky players that access the boardView after the game is over
		// from collapsing the game manager.
		// Note 1: The pointer is moved and no longer valid but the algo keeps a reference to the
		// real object which stays intact.
		// Note 2: BattleBoard lifetime is extended by the BoardDataImpl view so we only cache that.
		resourcePool.cacheResourcesForPlayer(_playerAName, std::move(playerAView));
		resourcePool.cacheResourcesForPlayer(_playerBName, std::move(playerBView));

		scoreBoard->updateWithGameResults(*gameResults, _playerAName, _playerBName, _boardName);
	}

	const string& SingleGameTask::playerAName() const
	{
		return _playerAName;
	}

	const string& SingleGameTask::playerBName() const
	{
		return _playerBName;
	}

	const string& SingleGameTask::boardName() const
	{
		return _boardName;
	}
}