#include "Scoreboard.h"
#include <unique_lock>

using std::lock_guard;
using std::unique_lock;

namespace battleship
{
	PlayerStatistics::PlayerStatistics(const string& aPlayerName) :
									   playerName(aPlayerName), pointsFor(0),
									   pointsAgainst(0), wins(0), loses(0), rating(0)
	{
	}

	PlayerStatistics::PlayerStatistics(const string& aPlayerName, int aPointsFor,
									   int aPointsAgainst, int aWins, int aLoses):
									   playerName(aPlayerName),
									   pointsFor(aPointsFor),
									   pointsAgainst(aPointsAgainst),
									   wins(aWins), loses(aLoses),
									   rating(aWins / (aWins + aLoses))
	{
	}

	shared_ptr<PlayerStatistics> PlayerStatistics::updateStatistics(int addedPointsFor,
																    int addedPointsAgainst,
																	bool isWin, bool isLose)
	{
		return std::make_shared<PlayerStatistics>(playerName,
												  pointsFor + addedPointsFor,
												  pointsAgainst + addedPointsAgainst,
												  wins + (isWin ? 1 : 0),
												  loses + (isLose ? 1 : 0));
	}

	RoundResults::RoundResults(int aRoundNum) : roundNum(aRoundNum)
	{
	}

	Scoreboard::Scoreboard(vector<string> players) :
		_playersPerRound(players.size())
	{
		for (const string& player : players)
		{
			_score.emplace(std::make_pair(player, PlayerStatistics(player)));
			_registeredMatches.emplace(player, 0);
		}
	}

	void Scoreboard::registerMatch(const string& playerA, const string& playerB)
	{
		_registeredMatches[playerA]++;
		_registeredMatches[playerB]++;
	}

	void Scoreboard::updatePlayerGameResults(PlayerEnum player, const string& playerName,
										     GameResults* results)
	{
		// Fetch current score for player
		PlayerStatistics& playerStatistics = _score.at(playerName);

		// Calculate round number for this player
		int playerRound = playerStatistics.wins + playerStatistics.loses;

		// Find RoundResults object for this round number
		auto roundEntry = _trackedMatches.find(playerRound);
		shared_ptr<RoundResults> roundResults = NULL;

		// If this is the first game in this round, create RoundResults
		if (roundEntry == _trackedMatches.end())
		{
			roundResults = std::make_shared<RoundResults>(playerRound);
			_trackedMatches.emplace(std::make_pair(playerRound, roundResults));
		}
		else
		{
			roundResults = roundEntry->second;
		}

		// Update RoundResults with the new player's statistics
		int pointsTo = (player == PlayerEnum::A) ? results->playerAPoints : results->playerBPoints;
		int pointsAgainst = (player == PlayerEnum::A) ? results->playerBPoints : results->playerAPoints;
		bool isWinner = (results->winner == player);
		bool isLoser = (results->winner != player) && (results->winner != PlayerEnum::NONE);
		auto newRoundStatistics = playerStatistics.updateStatistics(pointsTo, pointsAgainst, isWinner, isLoser);
		roundResults->playerStatistics.emplace(std::make_pair(playerName, newRoundStatistics));

		// Update player score with the newest record
		_score.emplace(std::make_pair(playerName, newRoundStatistics));

		// If this is the last update for this round, push the RoundResults to the approporiate
		// list so the reporter thread can wake up and print it
		if (roundResults->playerStatistics.size() == _playersPerRound)
		{
			unique_lock<mutex> lock(_roundResultsLock);
			_roundsResults.push_back(roundResults); // Guaranteed to happen before lock is released
			_roundResultsCV.notify_one();
		}
	}

	void Scoreboard::updateWithGameResults(shared_ptr<GameResults> results,
										   const string& playerAName,
										   const string& playerBName)
	{
		// Update the score table here.
		// Game result updates should be atomic so we lock here to protect the score table's integrity
		lock_guard<mutex> lock(_scoreLock);
		updatePlayerGameResults(PlayerEnum::A, playerAName, results.get());
		updatePlayerGameResults(PlayerEnum::B, playerBName, results.get());
	}

	int Scoreboard::getPlayerEnlistedMatches(const string& player) const
	{
		return _registeredMatches.at(player);
	}

	vector<shared_ptr<RoundResults>>& Scoreboard::getRoundResults()
	{
		return _roundsResults;
	}

	void Scoreboard::waitOnRoundResults(function<void()> callback)
	{
		// Wait here until new data appears in round results queue
		unique_lock<mutex> lock(_roundResultsLock);
		_roundResultsCV.wait(lock, [this] { return !(_roundsResults.empty()); });

		// Call callback in response to round results being ready
		callback();
	}
}