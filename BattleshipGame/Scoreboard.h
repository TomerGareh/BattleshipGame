#pragma once

#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <functional>
#include "GameManager.h"
#include "PlayerStatistics.h"

using std::shared_ptr;
using std::pair;
using std::vector;
using std::unordered_map;
using std::map;
using std::string;
using std::mutex;
using std::condition_variable;
using std::function;

namespace battleship
{
	struct PlayerStatisticsRatingSort {
		bool operator()(const PlayerStatistics& a, const PlayerStatistics& b) const
		{
			if (a.rating == b.rating)
			{
				return a.playerName > b.playerName; // Don't consider equal unless this is the same player
													// (for equal rating we choose some inner order between the players)
			}
			else
			{
				return a.rating > b.rating;
			}
		}
	};

	struct RoundResults
	{
		int roundNum;
		set<PlayerStatistics, PlayerStatisticsRatingSort> playerStatistics;

		RoundResults(int aRoundNum);
	};

	/**
	 *	Scoreboard for managing number of matches each player is enlisted in,
	 *  and the total number of points each player have accumulated so far.
	 *  This class does not validate, and assumes all player "strings" are valid players.
	 */
	class Scoreboard
	{
	public:
		Scoreboard(vector<string> players, int totalRounds);
		virtual ~Scoreboard() = default;

		/** Update the score table with the game results.
		 *  This method is thread safe.
		 */
		void updateWithGameResults(const GameResults& results,
								   const string& playerAName, const string& playerBName,
								   const string& boardName);

		/** A "queue" of round results for rounds that are finished being played.
		 *  Outside consumers are expected to pop entries from this data structure after processing them.
		 */
		vector<shared_ptr<RoundResults>>& getRoundResults();

		/** Waits on round results queue until new data is ready,
		 *  when it arrives - trigger the print results table function (as a callback)
		 */
		void waitOnRoundResults();

		/** Pop and print all round results ready in the _roundResults queue
		 */
		void processRoundResultsQueue();

	private:

		/** Minimal space allocated for player name in the table (visual parameter) */
		static constexpr int MIN_PLAYER_NAME_SIZE = 12;

		// Total rounds the competition should contain
		int _totalRounds;

		// Number of player entries that must be present for a round to count as finished
		int _playersPerRound;

		// A mutex lock to protect the score table during updates from multiple worker thread updates
		mutex _scoreLock;

		// A mutex lock to protect the roundResults table during access time
		mutex _roundResultsLock;

		// A predicate to notify listeners on the _roundResults queue that new data is ready
		condition_variable _roundResultsCV;

		// Current points & statistics for each player, contains the most up to date info about each player
		map<string, PlayerStatistics> _score;

		// Tracked matches data - 
		// key is round number
		// value is RoundResults (that accumulates data from finished games for each player for that round)
		unordered_map<int, shared_ptr<RoundResults>> _trackedMatches;

		// Contains results of finished rounds of games.
		// This data is ready for printing and is kept here until queried by the reporter thread.
		// Data is pushed to this vector only when the round is finished.
		// _roundResults is volatile to make sure it's handled before condition_variables are notified
		vector<shared_ptr<RoundResults>> _roundsResults;

		// Holds the longest player name encountered
		int _maxPlayerNameLength;

		// Holds the cursor position for the printing of the results
		pair<int, int> _resultsCursorPosition;

		/** Update the score table with the results for a single player from a single match
		 */
		void updatePlayerGameResults(PlayerEnum player, const string& playerName, const GameResults& results);

		/** Prints the round results in a formatted table to the console
		 */
		void printRoundResults(shared_ptr<RoundResults> roundResults);

		/** Get the next round for the player (to submit score to) */
		int getPlayerCurrentRound(const string& player) const;
	};
}
