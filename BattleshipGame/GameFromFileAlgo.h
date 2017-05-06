#pragma once

#include <string>
#include <vector>
#include <utility>
#include "IBattleshipGameAlgo.h"

using std::vector;
using std::string;

/** A player algorithm that relies on a predefined moves list that is read from an
 *	external file. The player will play until he wins / loses / his move list is over,
 *	in which case he will forfeit the game.
 */
class GameFromFileAlgo : public IBattleshipGameAlgo
{
public:
	GameFromFileAlgo();

	virtual ~GameFromFileAlgo() = default;

	GameFromFileAlgo(GameFromFileAlgo const&) = delete;	// Disable copying
	GameFromFileAlgo& operator=(GameFromFileAlgo const&) = delete;	// Disable copying (assignment)
	GameFromFileAlgo(GameFromFileAlgo&& other) noexcept = delete; // Disable moving
	GameFromFileAlgo& operator= (GameFromFileAlgo&& other) noexcept = delete; // Disable moving (assignment)

	/** Ignored by this algorithm (empty implementation) */
	void setBoard(int player, const char** board, int numRows, int numCols) override;

	/** Called once to allow init from files if needed
	 *  returns whether the init succeeded or failed */
	bool init(const string& path);

	/** Returns the next move stored in the attacks move file.
	 *	If no moves are left for the player to execute, FORFEIT is returned for each subsequent call.
	 */
	std::pair<int, int> attack() override;

	/** Ignored by this algorithm (empty implementation) */
	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

private:

	/** Games file extensions */
	static const string ATTACK_SUFFIX;

	/** A delimeter character to be used for attack files, to separate pair of coordinates */
	static const char COORDS_DELIM = ',';

	/** Saves the player number for this algorithm.
	 *	0 for player A, 1 for player B.
	 */
	int _playerNum;

	/** Board dimensions */
	int _numRows;
	int _numCols;

	/** A list of the player's predefined moves, as read from the external attack file */
	vector<std::pair<int, int>> _predefinedMoves;

	/** Populates the player's moves list with attack moves read from <filename> */
	void populateMovesFromFile(const string& filename);
};