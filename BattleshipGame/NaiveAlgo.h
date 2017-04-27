#pragma once

#include <string>
#include "IBattleshipGameAlgo.h"

using std::string;

class NaiveAlgo : public IBattleshipGameAlgo
{
public:
	NaiveAlgo();
	virtual ~NaiveAlgo() = default;

	void setBoard(int player, const char** board, int numRows, int numCols) override;

	bool init(const string& path);

	std::pair<int, int> attack() override;

	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

private:

	// The matrix which holds the visual data of the board game squares
	char** _visitedSquares = NULL;
};

