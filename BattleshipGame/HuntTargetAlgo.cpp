#include "HuntTargetAlgo.h"
#include "AlgoCommon.h"
#include <stdlib.h>
#include <algorithm>
#include <time.h>
#include <iostream>

using std::exception;
using std::cerr;
using std::endl;

const int HuntTargetAlgo::MAX_NUM_OF_DRAWS = 1000;
const AttackDirection HuntTargetAlgo::nonInPlaceDirections[] = {AttackDirection::RowPlus, AttackDirection::RowMinus,
																AttackDirection::ColPlus, AttackDirection::ColMinus,
																AttackDirection::DepthPlus, AttackDirection::DepthMinus};

HuntTargetAlgo::HuntTargetAlgo():  IBattleshipGameAlgo(),
								   playerId(0),
								   boardSize(std::make_tuple(0, 0, 0)),
								   visitedCoords({}),
								   lastAttackDirection(AttackDirection::InPlace)
{
}

HuntTargetAlgo::~HuntTargetAlgo()
{
}

void HuntTargetAlgo::setPlayer(int player)
{
	playerId = player;
}

// This function assumes that coord is inside the board
void HuntTargetAlgo::markRowNeighbors(Coordinate coord)
{
	if ((coord.row + 1) < std::get<0>(boardSize))
		visitedCoords.emplace(coord.row + 1, coord.col, coord.depth);
	if ((coord.row - 1) >= 0)
		visitedCoords.emplace(coord.row - 1, coord.col, coord.depth);
}

// This function assumes that coord is inside the board
void HuntTargetAlgo::markColNeighbors(Coordinate coord)
{
	if ((coord.col + 1) < std::get<1>(boardSize))
		visitedCoords.emplace(coord.row, coord.col + 1, coord.depth);
	if ((coord.col - 1) >= 0)
		visitedCoords.emplace(coord.row, coord.col - 1, coord.depth);
}

// This function assumes that coord is inside the board
void HuntTargetAlgo::markDepthNeighbors(Coordinate coord)
{
	if ((coord.depth + 1) < std::get<2>(boardSize))
		visitedCoords.emplace(coord.row, coord.col, coord.depth + 1);		
	if ((coord.depth - 1) >= 0)
		visitedCoords.emplace(coord.row, coord.col, coord.depth - 1);
}

void HuntTargetAlgo::setBoard(const BoardData& board)
{
	std::get<0>(boardSize) = board.rows();
	std::get<1>(boardSize) = board.cols();
	std::get<2>(boardSize) = board.depth();

	// Mark our ships and their surrounding as visited
	for (int i = 0; i < std::get<0>(boardSize); ++i)
	{
		for (int j = 0; j < std::get<1>(boardSize); ++j)
		{
			for (int k = 0; k < std::get<2>(boardSize); ++k)
			{
				Coordinate coord(i, j, k);
				if (board.charAt(coord) != static_cast<char>(BoardSquare::Empty))
				{
					visitedCoords.insert(coord);
					markRowNeighbors(coord);
					markColNeighbors(coord);
					markDepthNeighbors(coord);
				}
			}
		}
	}

	srand(static_cast<unsigned int>(time(nullptr)));	// Initialize random seed
}

Coordinate HuntTargetAlgo::searchUnvisitedCoord()
{
	for (int i = 0; i < std::get<0>(boardSize); ++i)
	{
		for (int j = 0; j < std::get<1>(boardSize); ++j)
		{
			for (int k = 0; k < std::get<2>(boardSize); ++k)
			{
				if (visitedCoords.find(Coordinate(i, j, k)) == visitedCoords.end())
					return Coordinate(i+1, j+1, k+1);
			}
		}
	}

	return NO_MORE_MOVES;
}

AttackDirection HuntTargetAlgo::getTargetDirection(targetsMapEntry targetIt)
{
	map<AttackDirection, int>& directionMap = targetIt->second;
	map<AttackDirection, int>::iterator maxDirection = std::max_element(directionMap.begin(), directionMap.end(),
		[](const pair<AttackDirection, int>& p1, const pair<AttackDirection, int>& p2) {
			// Search for the max element that isn't InPlace direction
			if (p1.first == AttackDirection::InPlace)
				return true;
			else if (p2.first == AttackDirection::InPlace)
				return false;
			else
				return (p1.second < p2.second);
		});
	return maxDirection->first;
}

bool HuntTargetAlgo::calcTargetNext(Coordinate& coord, AttackDirection direction, int size)
{
	switch (direction)
	{
	case AttackDirection::RowPlus:
	{
		if ((coord.row + size > std::get<0>(boardSize)) ||
			(visitedCoords.find(Coordinate(coord.row-1+size, coord.col-1, coord.depth-1)) != visitedCoords.end()))
			return false;
		coord.row += size;
		return true;
	}
	case AttackDirection::RowMinus:
	{
		if ((coord.row - size < 1) ||
			(visitedCoords.find(Coordinate(coord.row-1-size, coord.col-1, coord.depth-1)) != visitedCoords.end()))
			return false;
		coord.row -= size;
		return true;
	}
	case AttackDirection::ColPlus:
	{
		if ((coord.col + size > std::get<1>(boardSize)) ||
			(visitedCoords.find(Coordinate(coord.row-1, coord.col-1+size, coord.depth-1)) != visitedCoords.end()))
			return false;
		coord.col += size;
		return true;
	}
	case AttackDirection::ColMinus:
	{
		if ((coord.col - size < 1) ||
			(visitedCoords.find(Coordinate(coord.row-1, coord.col-1-size, coord.depth-1)) != visitedCoords.end()))
			return false;
		coord.col -= size;
		return true;
	}
	case AttackDirection::DepthPlus:
	{
		if ((coord.depth + size > std::get<2>(boardSize)) ||
			(visitedCoords.find(Coordinate(coord.row-1, coord.col-1, coord.depth-1+size)) != visitedCoords.end()))
			return false;
		coord.depth += size;
		return true;
	}
	case AttackDirection::DepthMinus:
	{
		if ((coord.depth - size < 1) ||
			(visitedCoords.find(Coordinate(coord.row-1, coord.col-1, coord.depth-1-size)) != visitedCoords.end()))
			return false;
		coord.depth -= size;
		return true;
	}
	default:
		return false;
	}
}

Coordinate HuntTargetAlgo::attack()
{
	try
	{
		Coordinate coord = NO_MORE_MOVES;
		if (targetsMap.empty())	// Hunt mode: we draw a random attack
		{
			int drawsCounter = 0;
			do {
				coord.row = rand() % std::get<0>(boardSize) + 1;	// In the range 1 to number of rows
				coord.col = rand() % std::get<1>(boardSize) + 1;	// In the range 1 to number of columns
				coord.depth = rand() % std::get<2>(boardSize) + 1;	// In the range 1 to number of depths
				drawsCounter++;
			} while ((visitedCoords.find(Coordinate(coord.row-1, coord.col-1, coord.depth-1)) != visitedCoords.end()) &&
					 (drawsCounter <= MAX_NUM_OF_DRAWS));
			
			lastAttackDirection = AttackDirection::InPlace;

			if (drawsCounter > MAX_NUM_OF_DRAWS)
				coord = searchUnvisitedCoord();
		}
		else	// Target mode: we try to attack around the targets that we already found
		{
			auto currTarget = targetsMap.begin();
			coord.row = currTarget->first.row + 1;		// 1 to number of rows
			coord.col = currTarget->first.col + 1;		// 1 to number of columns
			coord.depth = currTarget->first.depth + 1;	// 1 to number of depths
			bool foundAttack = false;
			while (!foundAttack)
			{
				lastAttackDirection = getTargetDirection(currTarget);
				int maxDirection = currTarget->second[lastAttackDirection];

				if (maxDirection == -1)	// This is a redundant target that came from the other player
				{						// We remove it to avoid infinite loop
					targetsMap.erase(currTarget);
					return attack();
				}

				foundAttack = calcTargetNext(coord, lastAttackDirection, maxDirection + 1);
				if (!foundAttack)	// This direction is no longer applicable
					currTarget->second[lastAttackDirection] = -1;
			}
		}

		AttackValidator validator;
		return validator(coord, std::get<0>(boardSize), std::get<1>(boardSize), std::get<2>(boardSize));
	}
	catch (const exception& e)
	{	// This should be a barrier that stops the app from failing.
		// If the algorithm fails, let it forfeit
		cerr << "Error: HuntTargetAlgo::attack failed on " << e.what() << endl;
		return NO_MORE_MOVES;
	}
}

int HuntTargetAlgo::getTargetSize(const map<AttackDirection, int>& targetSurround)
{
	int size = 0;
	for (const auto& directionSizePair : targetSurround)
	{
		if (directionSizePair.second != -1)
			size += directionSizePair.second;
	}
	return size;
}

targetsMapEntry HuntTargetAlgo::updateMapOnOtherAttack(Coordinate coord, AttackResult result)
{
	for (targetsMapEntry targetIt = targetsMap.begin(); targetIt != targetsMap.end(); ++targetIt)
	{
		Coordinate targetCoord = targetIt->first;
		AttackDirection direction = getTargetDirection(targetIt);

		auto checkCoordinatesMatch = [coord, targetCoord](AttackDirection direct, int size)->bool {
			return (((direct == AttackDirection::RowPlus) && (Coordinate(targetCoord.row + size + 1, targetCoord.col, targetCoord.depth) == coord))	||
					((direct == AttackDirection::RowMinus) && (Coordinate(targetCoord.row - size - 1, targetCoord.col, targetCoord.depth) == coord)) ||
					((direct == AttackDirection::ColPlus) && (Coordinate(targetCoord.row, targetCoord.col + size + 1, targetCoord.depth) == coord))	||
					((direct == AttackDirection::ColMinus) && (Coordinate(targetCoord.row, targetCoord.col - size - 1, targetCoord.depth) == coord)) ||
					((direct == AttackDirection::DepthPlus) && (Coordinate(targetCoord.row, targetCoord.col, targetCoord.depth + size + 1) == coord)) ||
					((direct == AttackDirection::DepthMinus) && (Coordinate(targetCoord.row, targetCoord.col, targetCoord.depth - size - 1) == coord)));
		};

		bool notInDirectionCase = false;
		for (const auto otherDirection : nonInPlaceDirections)
		{
			if ((otherDirection != direction) && checkCoordinatesMatch(otherDirection, 0))
			{
				notInDirectionCase = true;
				direction = otherDirection;
				break;
			}
		}

		int maxDirection = targetIt->second[direction];
		bool inDirectionCase = checkCoordinatesMatch(direction, maxDirection);

		if (inDirectionCase || notInDirectionCase)
		{
			if (result == AttackResult::Miss)
			{
				targetIt->second[direction] = -1;
			}
			else
			{
				targetIt->second[direction] += 1;
				lastAttackDirection = direction;
			}
			return targetIt;
		}
	}

	return targetsMap.end();
}

void HuntTargetAlgo::advanceInDirection(Coordinate& coord, AttackDirection direction, int size)
{
	switch (direction)
	{
	case AttackDirection::RowPlus:
	{
		coord.row += size;
		break;
	}
	case AttackDirection::RowMinus:
	{
		coord.row -= size;
		break;
	}
	case AttackDirection::ColPlus:
	{
		coord.col += size;
		break;
	}
	case AttackDirection::ColMinus:
	{
		coord.col -= size;
		break;
	}
	case AttackDirection::DepthPlus:
	{
		coord.depth += size;
		break;
	}
	case AttackDirection::DepthMinus:
	{
		coord.depth -= size;
		break;
	}
	default:
		break;
	}
}

void HuntTargetAlgo::removeRedundantTargets(Coordinate coord, AttackDirection direction)
{
	advanceInDirection(coord, direction, 1);

	for (targetsMapEntry targetIt = targetsMap.begin(); targetIt != targetsMap.end(); ++targetIt)
	{
		AttackDirection currDirection = getTargetDirection(targetIt);
		Coordinate advancedCoord = targetIt->first;
		advanceInDirection(advancedCoord, currDirection, targetIt->second[currDirection]);

		if ((targetIt->first == coord) || (advancedCoord == coord))
		{
			targetsMap.erase(targetIt);
			break;
		}
	}
}

void HuntTargetAlgo::notifyOnAttackInTarget(Coordinate moveZeroBased, targetsMapEntry target, AttackResult result)
{
	visitedCoords.insert(moveZeroBased);

	int currTargetSize = getTargetSize(target->second);

	if (currTargetSize == 2)	// We know now the orientation so we mark visited squares for the first hit
	{
		if (lastAttackDirection == AttackDirection::RowPlus)
		{
			Coordinate firstHit(moveZeroBased.row-1, moveZeroBased.col, moveZeroBased.depth);
			markColNeighbors(firstHit);
			markDepthNeighbors(firstHit);
		}
		else if (lastAttackDirection == AttackDirection::RowMinus)
		{
			Coordinate firstHit(moveZeroBased.row+1, moveZeroBased.col, moveZeroBased.depth);
			markColNeighbors(firstHit);
			markDepthNeighbors(firstHit);
		}
		else if (lastAttackDirection == AttackDirection::ColPlus)
		{
			Coordinate firstHit(moveZeroBased.row, moveZeroBased.col-1, moveZeroBased.depth);
			markRowNeighbors(firstHit);
			markDepthNeighbors(firstHit);
		}
		else if (lastAttackDirection == AttackDirection::ColMinus)
		{
			Coordinate firstHit(moveZeroBased.row, moveZeroBased.col+1, moveZeroBased.depth);
			markRowNeighbors(firstHit);
			markDepthNeighbors(firstHit);
		}
		else if (lastAttackDirection == AttackDirection::DepthPlus)
		{
			Coordinate firstHit(moveZeroBased.row, moveZeroBased.col, moveZeroBased.depth-1);
			markRowNeighbors(firstHit);
			markColNeighbors(firstHit);
		}
		else if (lastAttackDirection == AttackDirection::DepthMinus)
		{
			Coordinate firstHit(moveZeroBased.row, moveZeroBased.col, moveZeroBased.depth+1);
			markRowNeighbors(firstHit);
			markColNeighbors(firstHit);
		}
	}

	if ((result == AttackResult::Hit) && (currTargetSize > 1))
	{
		if ((lastAttackDirection == AttackDirection::RowPlus) || (lastAttackDirection == AttackDirection::RowMinus))
		{
			markColNeighbors(moveZeroBased);
			markDepthNeighbors(moveZeroBased);
		}
		else if ((lastAttackDirection == AttackDirection::ColPlus) || (lastAttackDirection == AttackDirection::ColMinus))
		{
			markRowNeighbors(moveZeroBased);
			markDepthNeighbors(moveZeroBased);
		}
		else if ((lastAttackDirection == AttackDirection::DepthPlus) || (lastAttackDirection == AttackDirection::DepthMinus))
		{
			markRowNeighbors(moveZeroBased);
			markColNeighbors(moveZeroBased);
		}
	}
	else if (result == AttackResult::Sink)
	{
		markRowNeighbors(moveZeroBased);
		markColNeighbors(moveZeroBased);
		markDepthNeighbors(moveZeroBased);

		// Mark the other edge as visited
		if ((lastAttackDirection == AttackDirection::RowPlus) && (moveZeroBased.row-currTargetSize >= 0))
		{
			visitedCoords.emplace(moveZeroBased.row-currTargetSize, moveZeroBased.col, moveZeroBased.depth);
		}
		else if ((lastAttackDirection == AttackDirection::RowMinus) && (moveZeroBased.row+currTargetSize < std::get<0>(boardSize)))
		{
			visitedCoords.emplace(moveZeroBased.row+currTargetSize, moveZeroBased.col, moveZeroBased.depth);
		}
		else if ((lastAttackDirection == AttackDirection::ColPlus) && (moveZeroBased.col-currTargetSize >= 0))
		{
			visitedCoords.emplace(moveZeroBased.row, moveZeroBased.col-currTargetSize, moveZeroBased.depth);
		}
		else if ((lastAttackDirection == AttackDirection::ColMinus) && (moveZeroBased.col+currTargetSize < std::get<1>(boardSize)))
		{
			visitedCoords.emplace(moveZeroBased.row, moveZeroBased.col+currTargetSize, moveZeroBased.depth);
		}
		else if ((lastAttackDirection == AttackDirection::DepthPlus) && (moveZeroBased.depth-currTargetSize >= 0))
		{
			visitedCoords.emplace(moveZeroBased.row, moveZeroBased.col, moveZeroBased.depth-currTargetSize);
		}
		else if ((lastAttackDirection == AttackDirection::DepthMinus) && (moveZeroBased.depth+currTargetSize < std::get<2>(boardSize)))
		{
			visitedCoords.emplace(moveZeroBased.row, moveZeroBased.col, moveZeroBased.depth+currTargetSize);
		}

		AttackDirection targetDirection = getTargetDirection(target);
		targetsMap.erase(target);
		removeRedundantTargets(moveZeroBased, targetDirection);
	}
}

void HuntTargetAlgo::notifyOnAttackResult(int player, Coordinate move, AttackResult result)
{
	try
	{
		Coordinate moveZeroBased(move.row-1, move.col-1, move.depth-1);

		if (targetsMap.empty())	// The attack comes from Hunt mode or other player
		{
			if ((player == playerId) ||
				((player != playerId) && (visitedCoords.find(moveZeroBased) == visitedCoords.end())))
			{
				visitedCoords.insert(moveZeroBased);

				if (result == AttackResult::Hit)	// New target
				{
					map<AttackDirection, int> newTargetSurround = {{AttackDirection::InPlace, 1}, 
																   {AttackDirection::RowPlus, 0}, {AttackDirection::RowMinus, 0}, 
																   {AttackDirection::ColPlus, 0}, {AttackDirection::ColMinus, 0},
																   {AttackDirection::DepthPlus, 0}, {AttackDirection::DepthMinus, 0}};
					targetsMap.emplace(move, newTargetSurround);
				}
				else if (result == AttackResult::Sink)	// In case of rubber boat
				{
					markRowNeighbors(moveZeroBased);
					markColNeighbors(moveZeroBased);
					markDepthNeighbors(moveZeroBased);
				}
			}
		}
		else
		{
			auto currTarget = targetsMap.begin();

			// The attack comes from other player
			if (player != playerId)
			{
				if (visitedCoords.find(moveZeroBased) == visitedCoords.end())
				{
					visitedCoords.insert(moveZeroBased);
					// Search for existing target
					targetsMapEntry updateMapResult = updateMapOnOtherAttack(moveZeroBased, result);
					if ((updateMapResult == targetsMap.end()) || (result == AttackResult::Miss))
					{
						if (result == AttackResult::Hit)	// New target
						{
							map<AttackDirection, int> newTargetSurround = {{AttackDirection::InPlace, 1},
																		   {AttackDirection::RowPlus, 0}, {AttackDirection::RowMinus, 0},
																		   {AttackDirection::ColPlus, 0}, {AttackDirection::ColMinus, 0},
																		   {AttackDirection::DepthPlus, 0}, {AttackDirection::DepthMinus, 0}};
							targetsMap.emplace(move, newTargetSurround);
						}
						return;
					}
					currTarget = updateMapResult;
				}
				else
				{
					return;
				}
			}
			else if (result == AttackResult::Miss)	// player == playerId
			{
				currTarget->second[lastAttackDirection] = -1;
			}
			else	// (player == playerId) && (result != AttackResult::Miss)
			{
				currTarget->second[lastAttackDirection] += 1;
			}

			// The attack comes from Target mode (or self attack of the opponent)
			notifyOnAttackInTarget(moveZeroBased, currTarget, result);
		}
	}
	catch (const exception& e)
	{	// This should be a barrier that stops the app from failing
		cerr << "Error: HuntTargetAlgo::notifyOnAttackResults failed on " << e.what() << endl;
	}
}

ALGO_API IBattleshipGameAlgo* GetAlgorithm()
{
	return new HuntTargetAlgo();
}