#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include "IBattleshipGameAlgo.h"

using std::string;
using std::pair;
using std::vector;
using std::map;

enum class AttackDirection : int
{
	InPlace = 0,
	Right = 1,
	Left = 2,
	Up = 3,
	Down = 4
};

class HuntTargetAlgo : public IBattleshipGameAlgo
{
public:
	HuntTargetAlgo();
	~HuntTargetAlgo();

	HuntTargetAlgo(HuntTargetAlgo const&) = delete;	// Disable copying
	HuntTargetAlgo& operator=(HuntTargetAlgo const&) = delete;	// Disable copying (assignment)
	HuntTargetAlgo(HuntTargetAlgo&& other) noexcept = delete; // Disable moving
	HuntTargetAlgo& operator= (HuntTargetAlgo&& other) noexcept = delete; // Disable moving (assignment)

	void setBoard(int player, const char** board, int numRows, int numCols) override;

	bool init(const string& path) override;	// Ignored by this algorithm (always return true)

	pair<int, int> attack() override;

	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

private:
	int playerId;

	pair<int, int> boardSize;

	// A board of booleans, indicating the squares we already visited
	bool** visitedBoard;
	
	// Our last attack direction. If the last attack was in Hunt mode this field is not relevant
	AttackDirection lastAttackDirection;

	// A map from the pending targets to their surrounding. 
	// The surrounding is a vector of ints, representing the target square itself and the four directions according
	// to AttackDirection enum. In each cell we keep the size of progression in that direction. Size of -1 is
	// indicating that we failed to attack in that direction.
	map<pair<int, int>, vector<int>> targetsMap;

	void allocateVisitedBoard();

	// row and col are in the range 0 to board size - 1
	void markRightLeftAsVisited(int row, int col);
	
	// row and col are in the range 0 to board size - 1
	void markUpDownAsVisited(int row, int col);

	// Check if the try to attack around a target is a valid attack, i.e. doesn't exceed the borders of the board
	// and not yet visited.
	// row and col are in the range 1 to board size
	bool calcTargetNext(int& row, int& col, AttackDirection direction, int size);

	int getTargetSize(vector<int>* targetSurround);
};