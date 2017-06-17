#include "PlayerStatistics.h"

namespace battleship
{
	PlayerStatistics::PlayerStatistics(const string& aPlayerName) :
		playerName(aPlayerName), pointsFor(0),
		pointsAgainst(0), wins(0), loses(0), ties(0), rating(0)
	{
	}

	PlayerStatistics::PlayerStatistics(const string& aPlayerName, int aPointsFor,
		int aPointsAgainst, int aWins, int aLoses, int aTies) :
		playerName(aPlayerName),
		pointsFor(aPointsFor),
		pointsAgainst(aPointsAgainst),
		wins(aWins), loses(aLoses), ties(aTies),
		rating(getPlayerRating(aWins, aLoses))
	{
	}

	PlayerStatistics::PlayerStatistics(PlayerStatistics const& other) :
		playerName(other.playerName), pointsFor(other.pointsFor),
		pointsAgainst(other.pointsAgainst), wins(other.wins), loses(other.loses), ties(other.ties), rating(other.rating)
	{
	}

	PlayerStatistics& PlayerStatistics::operator=(PlayerStatistics const& other)
	{
		// Check for self-assignment
		if (&other != this)
		{
			playerName = other.playerName;
			pointsFor = other.pointsFor;
			pointsAgainst = other.pointsAgainst;
			wins = other.wins;
			loses = other.loses;
			ties = other.ties;
			rating = other.rating;
		}

		return *this;
	}

	PlayerStatistics::PlayerStatistics(PlayerStatistics&& other) noexcept:
	playerName(std::move(other.playerName)), pointsFor(other.pointsFor),
		pointsAgainst(other.pointsAgainst), wins(other.wins), loses(other.loses), ties(other.ties), rating(other.rating)
	{
	}

	PlayerStatistics& PlayerStatistics::operator= (PlayerStatistics&& other) noexcept
	{
		playerName = std::move(other.playerName);
		pointsFor = other.pointsFor;
		pointsAgainst = other.pointsAgainst;
		wins = other.wins;
		loses = other.loses;
		ties = other.ties;
		rating = other.rating;

		other.pointsFor = 0;
		other.pointsAgainst = 0;
		other.wins = 0;
		other.loses = 0;
		other.ties = 0;
		other.rating = 0;

		return *this;
	}

	PlayerStatistics PlayerStatistics::updateStatistics(int addedPointsFor,
		int addedPointsAgainst,
		bool isWin, bool isLose) const
	{
		return PlayerStatistics(playerName,
			pointsFor + addedPointsFor,
			pointsAgainst + addedPointsAgainst,
			wins + (isWin ? 1 : 0),
			loses + (isLose ? 1 : 0),
			ties + ((!isWin && !isLose) ? 1 : 0));
	}

	float PlayerStatistics::getPlayerRating(int wins, int loses)
	{
		return 100 * static_cast<float>(wins) / static_cast<float>(wins + loses);
	}

	int PlayerStatistics::getRoundsPlayed() const
	{
		return wins + loses + ties;
	}
}
