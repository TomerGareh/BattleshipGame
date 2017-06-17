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
		// Register match so the scoreboard remembers how many matches each player have signed up for
		_scoreBoard->registerMatch(playerAName, playerBName);

		// Keep game number for each player, this helps sorting the games order to make it fair among players
		_playerAGameNum = _scoreBoard->getPlayerEnlistedMatches(playerAName);
		_playerBGameNum = _scoreBoard->getPlayerEnlistedMatches(playerBName);

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
		_scoreBoard->updateWithGameResults(gameResults, _playerAName, _playerBName);

		string gameResultStr = (gameResults->winner == PlayerEnum::A) ?  "Player A wins" :
							   ((gameResults->winner == PlayerEnum::B) ? "Player B wins" :
																		 "Tie");
							    
		string msg = "Game finished between Player A: " + _playerAName +
					 " (" + std::to_string(gameResults->playerAPoints) +
					 " pts) and Player B: " + _playerBName +
					 " (" + std::to_string(gameResults->playerBPoints) +
					 " pts) on board: " + _boardName + ". Game result: " + gameResultStr;
					 ;
		Logger::getInstance().log(Severity::INFO_LEVEL, msg);
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

	int SingleGameTask::playerAGameNum() const
	{
		return _playerAGameNum;
	}

	int SingleGameTask::playerBGameNum() const
	{
		return _playerBGameNum;
	}

	/** For reordering SingleGameTask by player's game time.
	 *	Helps sorting SingleGameTasks in the priority queue to keep the competition fair for all player's
	 *  chance of playing.
	 */
	bool operator<(const shared_ptr<SingleGameTask> lhs, const shared_ptr<SingleGameTask> rhs)
	{
		// Game with the player that played the least amount of games wins higher priority
		int lowestLhsPlayer = min(lhs->playerAGameNum(), lhs->playerBGameNum());
		int lowestRhsPlayer = min(rhs->playerAGameNum(), rhs->playerBGameNum());

		if (lowestLhsPlayer != lowestRhsPlayer)
		{
			return lowestLhsPlayer < lowestRhsPlayer;
		}
		else
		{
			int highestLhsPlayer = max(lhs->playerAGameNum(), lhs->playerBGameNum());
			int highestRhsPlayer = max(rhs->playerAGameNum(), rhs->playerBGameNum());

			return highestLhsPlayer < highestRhsPlayer;
		}
	}
}