#pragma once

#include "IBattleshipGameAlgo.h"
#include <vector>
#include <utility>
#include <string>

using std::vector;
using std::string;

/** A player algorithm that relies on a predefined moves list that is read from an
 *	external file. The player will play until he wins / loses / his move list is over,
 *	in which case he will forfeit the game.
 */
class GameFromFileAlgo : public IBattleshipGameAlgo
{
public:
	GameFromFileAlgo(const string& attackFile);

	virtual ~GameFromFileAlgo() = default;

	/** Ignored by this algorithm (empty implementation) */
	void setBoard(const char** board, int numRows, int numCols) override;

	/** Returns the next move stored in the attacks move file.
	 *	If no moves are left for the player to execute, FORFEIT is returned for each subsequent call.
	 */
	std::pair<int, int> attack() override;

	/** Ignored by this algorithm (empty implementation) */
	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

private:
	/** A list of the player's predefined moves, as read from the external attack file */
	vector<std::pair<int, int>> _predefinedMoves;

	/** Populates the player's moves list with attack moves read from <filename> */
	void populateMovesFromFile(const string& filename);
};