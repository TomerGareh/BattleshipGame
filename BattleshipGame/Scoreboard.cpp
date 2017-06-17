#include "Scoreboard.h"
#include "Logger.h"
#include "IOUtil.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <sstream>
#include <windows.h>

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
using std::left;

namespace battleship
{
	RoundResults::RoundResults(int aRoundNum) : roundNum(aRoundNum)
	{
	}

	Scoreboard::Scoreboard(vector<string> players, int totalRounds) :
		_totalRounds(totalRounds),
		_playersPerRound(static_cast<int>(players.size())), // Safe to assume that the number of players
														    // does not overflow integer 
		_resultsCursorPosition(std::make_pair(0, 0))
	{
		// Save max player name for score results table formatting
		_maxPlayerNameLength = MIN_PLAYER_NAME_SIZE;

		for (const string& player : players)
		{
			// Store initialized player score information
			_score.emplace(std::make_pair(player, PlayerStatistics(player)));

			// Query for the longest name
			if (_maxPlayerNameLength < player.length())
				_maxPlayerNameLength = static_cast<int>(player.length()); // Name is expected to be short enough
																		  // to safely cast from size_t to int
		}

		_maxPlayerNameLength += 2; // Apply some spacing between tabs in printed scoreboard
	}

	int Scoreboard::getPlayerCurrentRound(const string& player) const
	{
		// Fetch current score for player
		const PlayerStatistics& playerStatistics = _score.at(player);

		// Calculate round number for this player, start from round #1
		int playerRound = playerStatistics.getRoundsPlayed() + 1;

		return playerRound;
	}

	void Scoreboard::updatePlayerGameResults(PlayerEnum player, const string& playerName, GameResults* results)
	{
		// Calculate round number for this player, start from round #1
		int playerRound = getPlayerCurrentRound(playerName);

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

		// Fetch current score for player
		PlayerStatistics& playerStatistics = _score.at(playerName);

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
										   const string& playerBName,
										   const string& boardName)
	{
		// Update the score table here.
		// Game result updates should be atomic so we lock here to protect the score table's integrity
		lock_guard<mutex> lock(_scoreLock);

		string gameResultStr = (results->winner == PlayerEnum::A) ?  "Player A wins" :
							   ((results->winner == PlayerEnum::B) ? "Player B wins" :
																	 "Tie");

		int playerARound = getPlayerCurrentRound(playerAName);
		int playerBRound = getPlayerCurrentRound(playerBName);
		string msg = "Game finished between Player A: " + playerAName +
					 " (Round #" + std::to_string(playerARound) + ", " + std::to_string(results->playerAPoints) +
					 " pts) and Player B: " + playerBName +
					 " (Round #" + std::to_string(playerBRound) + ", " + std::to_string(results->playerBPoints) +
					 " pts) on board: " + boardName + ". Game result: " + gameResultStr;

		Logger::getInstance().log(Severity::INFO_LEVEL, msg);

		updatePlayerGameResults(PlayerEnum::A, playerAName, results.get());
		updatePlayerGameResults(PlayerEnum::B, playerBName, results.get());
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

		ss << left << setw(8) << "#"
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

		if (roundResults->roundNum == 1)
		{
			COORD currPosition(IOUtil::getConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE)));
			_resultsCursorPosition.first = currPosition.Y;
			_resultsCursorPosition.second = currPosition.X;
		}
		IOUtil::gotoxy(_resultsCursorPosition.first, _resultsCursorPosition.second);
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