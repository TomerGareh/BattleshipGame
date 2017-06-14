#pragma once

#include <memory>
#include "Scoreboard.h"
#include "GameManager.h"
#include "WorkerThreadResourcePool.h"

using std::shared_ptr;
using std::string;

namespace battleship
{
	/** Single game task for running a single game */
	class SingleGameTask
	{
	public:
		SingleGameTask(const string& playerAName, const string& playerBName,
					   const string& boardName, Scoreboard* scoreBoard);
		virtual ~SingleGameTask() = default;
		
		/** Run single game betwen playerA and playerB on stored board.
		 *  This method will allocate the resources needed to run the game if not already cached for
		 *  this worker thread, and then run the game and update the scoreboard with the results
		 */
		void run(const GameManager& gameManager, WorkerThreadResourcePool& resourcePool) const;

		const string& playerAName() const;
		const string& playerBName() const;
		const string& boardName() const;
		int playerAGameNum() const;
		int playerBGameNum() const;

	private:

		// Player algo path identifiers
		string _playerAName;
		string _playerBName;

		// Board path identifier
		string _boardName;

		// Return the number of game this is of each player (first, second, etc)
		int _playerAGameNum;
		int _playerBGameNum;

		// Scoreboard that keeps track of the game results
		Scoreboard* _scoreBoard;
	};
}