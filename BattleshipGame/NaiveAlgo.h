#pragma once

#include <string>
#include "IBattleshipGameAlgo.h"
#include <utility>

using std::string;
using std::pair;

class NaiveAlgo : public IBattleshipGameAlgo
{
public:
	const pair<int, int> NO_MORE_MOVES = std::make_pair(-1, -1);

	NaiveAlgo();
	NaiveAlgo(NaiveAlgo const&) = delete;	// Disable copying
	NaiveAlgo& operator=(NaiveAlgo const&) = delete;	// Disable copying (assignment)
	NaiveAlgo(NaiveAlgo&& other) noexcept; // Enable moving
	NaiveAlgo& operator= (NaiveAlgo&& other) noexcept; // Enable moving (assignment)
	virtual ~NaiveAlgo() noexcept;

	void disposeAllocatedResources() noexcept;

	void setBoard(int player, const char** board, int numRows, int numCols) override;

	bool init(const string& path);

	std::pair<int, int> attack() override;

	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

	pair<int, int> getNextTarget();

private:

	enum class NaiveSquareStatus : char
	{
		Unknown = '?',
		Visited = 'V'
	};

	// The matrix which holds the visual data of the board game squares
	char** _visitedSquares;
	int _numOfRows;
	int _numOfCols;

	int _nextRow;
	int _nextCol;
};

