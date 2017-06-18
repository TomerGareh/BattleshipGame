#pragma once

#include <memory>
#include "Scoreboard.h"
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
					   const string& boardName);
		virtual ~SingleGameTask() = default;

		/** Run single game betwen playerA and playerB on stored board.
		 *  This method will allocate the resources needed to run the game if not already cached for
		 *  this worker thread, and then run the game and update the scoreboard with the results
		 */
		void run(WorkerThreadResourcePool& resourcePool, Scoreboard* scoreBoard) const;

		const string& playerAName() const;
		const string& playerBName() const;
		const string& boardName() const;

	private:

		// Player algo path identifiers
		string _playerAName;
		string _playerBName;

		// Board path identifier
		string _boardName;
	};
}