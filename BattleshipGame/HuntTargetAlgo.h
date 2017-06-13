#pragma once

#include <string>
#include <tuple>
#include <vector>
#include <map>
#include <unordered_set>
#include "IBattleshipGameAlgo.h"
#include "AlgoCommon.h"

using std::string;
using std::tuple;
using std::vector;
using std::map;
using std::unordered_set;

enum class AttackDirection
{
	InPlace,
	RowPlus,
	RowMinus,
	ColPlus,
	ColMinus,
	DepthPlus,
	DepthMinus
};

using targetsMapEntry = map<Coordinate, map<AttackDirection, int>>::iterator;

class HuntTargetAlgo : public IBattleshipGameAlgo
{
public:
	HuntTargetAlgo();
	~HuntTargetAlgo();

	HuntTargetAlgo(HuntTargetAlgo const&) = delete;	// Disable copying
	HuntTargetAlgo& operator=(HuntTargetAlgo const&) = delete;	// Disable copying (assignment)
	HuntTargetAlgo(HuntTargetAlgo&& other) noexcept = delete; // Disable moving
	HuntTargetAlgo& operator= (HuntTargetAlgo&& other) noexcept = delete; // Disable moving (assignment)

	void setPlayer(int player) override;

	void setBoard(const BoardData& board) override;

	Coordinate attack() override;

	void notifyOnAttackResult(int player, Coordinate move, AttackResult result) override;

private:
	static const int MAX_NUM_OF_DRAWS;
	static const AttackDirection nonInPlaceDirections[];

	int playerId;

	tuple<int, int, int> boardSize;

	// An unordered set of the coordinates that have already been visited (by us or by the opponent)
	unordered_set<Coordinate, CoordinateHash> visitedCoords;
	
	// Our last attack direction. If the last attack was in Hunt mode this field is not relevant
	AttackDirection lastAttackDirection;

	// A map from the pending targets to their surrounding. 
	// The surrounding itself is also a map from the attack directions to the size of progression in that direction.
	// Size of -1 is indicating that we failed to attack in that direction.
	map<Coordinate, map<AttackDirection, int>> targetsMap;

	// coord is in the range 0 to board size - 1
	void markRowNeighbors(Coordinate coord);
	
	// coord is in the range 0 to board size - 1
	void markColNeighbors(Coordinate coord);

	// coord is in the range 0 to board size - 1
	void markDepthNeighbors(Coordinate coord);

	AttackDirection getTargetDirection(targetsMapEntry targetIt);

	int getTargetSize(const map<AttackDirection, int>& targetSurround);

	void advanceInDirection(Coordinate& coord, AttackDirection direction, int size);

	// Search for an unvisited coordinate in 'visitedCoords'. The returned coordiante is in the range 1 to board size.
	// If no square was found, battleship::NO_MORE_MOVES is returned.
	Coordinate searchUnvisitedCoord();

	// Check if the attempt to attack around a target is a valid attack, i.e. doesn't exceed the borders of the board
	// and not yet visited.
	// coord is in the range 1 to board size.
	bool calcTargetNext(Coordinate& coord, AttackDirection direction, int size);

	// coord is in the range 0 to board size - 1
	targetsMapEntry updateMapOnOtherAttack(Coordinate coord, AttackResult result);

	// coord is in the range 0 to board size - 1
	void removeRedundantTargets(Coordinate coord, AttackDirection direction);

	// Handle the case that the attack came from Target mode (or a self attack of the opponent), i.e. the attack belongs
	// to a target in 'targetsMap'
	void notifyOnAttackInTarget(Coordinate moveZeroBased, targetsMapEntry target, AttackResult result);
};