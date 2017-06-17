#pragma once

#include <string>

using std::string;

namespace battleship
{
	/** Stores player's statistics at a given point in time in the competition */
	class PlayerStatistics
	{
	public:
		PlayerStatistics(const string& aPlayerName);
		PlayerStatistics(const string& aPlayerName, int aPointsFor, int aPointsAgainst,
						 int aWins, int aLoses, int aTies);
		virtual ~PlayerStatistics() noexcept = default;
		PlayerStatistics(PlayerStatistics const& other); // Enable copying
		PlayerStatistics& operator=(PlayerStatistics const& other); // Enable copying (assignment)
		PlayerStatistics(PlayerStatistics&& other) noexcept; // Move constructor
		PlayerStatistics& operator= (PlayerStatistics&& other) noexcept; // Move assignment operator

																		 /** Get new statistics based on latest results and past statistics of the player */
		PlayerStatistics updateStatistics(int addedPointsFor, int addedPointsAgainst,
			bool isWin, bool isLose) const;

		/** Number of rounds the player have played so far */
		int getRoundsPlayed();

		// Components of player's statistics
		string playerName;
		int pointsFor;
		int pointsAgainst;
		int wins;
		int loses;
		int ties;
		float rating;

	private:
		static float getPlayerRating(int wins, int loses, int ties);
	};
}