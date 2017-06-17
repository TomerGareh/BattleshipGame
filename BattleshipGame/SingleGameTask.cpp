#include "SingleGameTask.h"
#include "BattleBoard.h"
#include "Logger.h"
#include <algorithm>

using std::min;
using std::max;

namespace battleship
{
	SingleGameTask::SingleGameTask(const string& playerAName, const string& playerBName,
								   const string& boardName, Scoreboard* scoreBoard):
		_playerAName(playerAName),
		_playerBName(playerBName),
		_boardName(boardName),
		_scoreBoard(scoreBoard)
	{
		string msg = "Created game between Player A: " + _playerAName +
					 " and Player B: " + _playerBName +
					 " on board: " + _boardName + ".";
		Logger::getInstance().log(Severity::DEBUG_LEVEL, msg);
	}

	void SingleGameTask::run(const GameManager& gameManager, WorkerThreadResourcePool& resourcePool) const
	{
		// Load resources
		auto playerA = resourcePool.requestAlgo(_playerAName);
		auto playerB = resourcePool.requestAlgo(_playerBName);
		auto board = resourcePool.requestBoard(_boardName);

		if ((playerA == nullptr) || (playerA == nullptr) || (playerA == nullptr))
		{
			string msg = "Error: Can't start a game between Player A: " + _playerAName +
						 " and Player B: " + _playerBName +
					     " on board: " + _boardName + " due to invalid resources";
			Logger::getInstance().log(Severity::ERROR_LEVEL, msg);
			return;
		}

		Logger::getInstance().log(Severity::DEBUG_LEVEL,
								  "Game started between Player A: " + _playerAName +
								  " and Player B: " + _playerBName + " on board: " + _boardName + ".");

		// Run a single game and update scoreboard with results
		auto gameResults = gameManager.runGame(board, playerA, playerB);

		_scoreBoard->updateWithGameResults(gameResults, _playerAName, _playerBName, _boardName);
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