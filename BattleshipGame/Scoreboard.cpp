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
using std::fixed;
using std::cout;
using std::endl;
using std::to_string;
using std::stringstream;

namespace battleship
{
	RoundResults::RoundResults(int aRoundNum) : roundNum(aRoundNum)
	{
	}

	Scoreboard::Scoreboard(vector<string> players, int totalRounds) :
		_totalRounds(totalRounds),
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

		_maxPlayerNameLength += 2; // Apply some spacing between tabs in printed scoreboard
	}

	void Scoreboard::registerMatch(const string& playerA, const string& playerB)
	{
		_registeredMatches[playerA]++;
		_registeredMatches[playerB]++;
	}

	void Scoreboard::updatePlayerGameResults(PlayerEnum player, const string& playerName, GameResults* results)
	{
		// Fetch current score for player
		PlayerStatistics& playerStatistics = _score.at(playerName);

		// Calculate round number for this player, start from round #1
		int playerRound = playerStatistics.getRoundsPlayed() + 1;

		// Find RoundResults object for this round number
		auto roundEntry = _trackedMatches.find(playerRound);
		shared_ptr<RoundResults> roundResults;

		// If this is the first game in this round, create RoundResults
		if (roundEntry == _trackedMatches.end())
		{
			roundResults = std::make_shared<RoundResults>(playerRound);
			_trackedMatches.emplace(std::make_pair(playerRound, roundResults));
			Logger::getInstance().log(Severity::DEBUG_LEVEL, "Round " + to_string(playerRound) + " started (1 game done).");
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
		auto scoreIt = _score.find(playerName);
		if (scoreIt != _score.end())
		{
			scoreIt->second = std::move(newRoundStatistics);
		}
		else
		{
			Logger::getInstance().log(Severity::ERROR_LEVEL, "Illegal state in scoretable - player data is missing.");
		}

		// If this is the last update for this round, push the RoundResults to the approporiate
		// list so the reporter thread can wake up and print it
		if (roundResults->playerStatistics.size() == _playersPerRound)
		{
			unique_lock<mutex> lock(_roundResultsLock);
			Logger::getInstance().log(Severity::INFO_LEVEL, "Round " + to_string(playerRound) + " finished.");
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
		// Use string stream so log "block" will be printed in an atomic manner
		stringstream ss;

		ss << "Results for round " << to_string(roundResults->roundNum) << 
			  "/" << to_string(_totalRounds) << endl;

		ss << setw(8) << "#"
		   << setw(_maxPlayerNameLength) << "Team Name"
		   << setw(8) << "Wins"
		   << setw(8) << "Losses"
		   << setw(8) << "%"
		   << setw(8) << "Pts For"
		   << setw(12) << "Pts Against" << endl;

		int place = 1;

		// RoundsResults are already sorted by player's rating
		for (const auto& playerStats : roundResults->playerStatistics)
		{
			string placeStr = to_string(place) + ".";
			ss << setw(8) << placeStr
			   << setw(_maxPlayerNameLength) << playerStats.playerName
			   << setw(8) << playerStats.wins
			   << setw(8) << playerStats.loses
			   << setw(8) << setprecision(2) << fixed << playerStats.rating
			   << setw(8) << playerStats.pointsFor
			   << setw(12) << playerStats.pointsAgainst << endl;
			place++;
		}

		Logger::getInstance().log(Severity::INFO_LEVEL, ss.str(), true); // true = Print to log & console
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