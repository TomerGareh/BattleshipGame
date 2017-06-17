#include "Scoreboard.h"
#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <sstream>

using std::lock_guard;
using std::unique_lock;
using std::min;
using std::setw;
using std::setprecision;
using std::cout;
using std::endl;
using std::to_string;
using std::stringstream;

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
									   rating(static_cast<float>(aWins) / static_cast<float>(aWins + aLoses))
	{
	}

	PlayerStatistics PlayerStatistics::updateStatistics(int addedPointsFor,
														int addedPointsAgainst,
														bool isWin, bool isLose) const
	{
		return PlayerStatistics(playerName,
								pointsFor + addedPointsFor,
								pointsAgainst + addedPointsAgainst,
								wins + (isWin ? 1 : 0),
								loses + (isLose ? 1 : 0));
	}

	RoundResults::RoundResults(int aRoundNum) : roundNum(aRoundNum)
	{
	}

	Scoreboard::Scoreboard(vector<string> players) :
		_playersPerRound(static_cast<int>(players.size())) // Safe to assume that the number of players
														   // does not overflow integer 
	{
		// Save max player name for score results table formatting
		_maxPlayerNameLength = MIN_PLAYER_NAME_SIZE;

		for (const string& player : players)
		{
			// Store initialized player score information
			_score.emplace(std::make_pair(player, PlayerStatistics(player)));
			_registeredMatches.emplace(player, 0);

			// Query for the longest name
			if (_maxPlayerNameLength < player.length())
				_maxPlayerNameLength = static_cast<int>(player.length()); // Name is expected to be short enough
																		  // to safely cast from size_t to int
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
		shared_ptr<RoundResults> roundResults;

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
		roundResults->playerStatistics.emplace(PlayerStatistics(newRoundStatistics)); // Save a copy in round results

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

	void Scoreboard::printRoundResults(shared_ptr<RoundResults> roundResults) const
	{
		Logger::getInstance().log(Severity::INFO_LEVEL,
								  "Results for round " +
								  to_string(roundResults->roundNum) + ":");

		stringstream ss;

		ss << setw(8) << "#"
		   << setw(_maxPlayerNameLength) << "Team Name"
		   << setw(8) << "Wins"
		   << setw(8) << "Losses"
		   << setw(8) << "%"
		   << setw(8) << "Pts For"
		   << setw(12) << "Pts Against";

		cout << ss.str() << endl;
		Logger::getInstance().log(Severity::INFO_LEVEL, ss.str());
		int place = 1;

		// RoundsResults are already sorted by player's rating
		for (const auto& playerStats : roundResults->playerStatistics)
		{
			ss.str("");
			string placeStr = place + ".";
			ss << setw(8) << placeStr
			   << setw(_maxPlayerNameLength) << playerStats.playerName
			   << setw(8) << playerStats.wins
			   << setw(8) << playerStats.loses
			   << setw(8) << setprecision(2) << playerStats.rating
			   << setw(8) << playerStats.pointsFor
			   << setw(12) << playerStats.pointsAgainst;
			cout << ss.str() << endl;
			Logger::getInstance().log(Severity::INFO_LEVEL, ss.str());
			place++;
		}
	}

	void Scoreboard::processRoundResultsQueue()
	{
		// Print all ready round results
		// After they are printed, we pop this data from the queue
		while (!_roundsResults.empty())
		{
			auto nextResults = _roundsResults.front();
			_roundsResults.pop_back();
			printRoundResults(nextResults);
		}
	}

	void Scoreboard::waitOnRoundResults()
	{
		// Wait here until new data appears in round results queue
		unique_lock<mutex> lock(_roundResultsLock);
		Logger::getInstance().log(Severity::INFO_LEVEL, "Main thread going to sleep until new results arrive.");
		_roundResultsCV.wait(lock, [this] { return !(_roundsResults.empty()); });

		Logger::getInstance().log(Severity::INFO_LEVEL, "Main thread woke up to handle new round results.");

		processRoundResultsQueue();
	}
}