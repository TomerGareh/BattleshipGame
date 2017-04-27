#pragma once

#include "string"
#include "IBattleshipGameAlgo.h"

class HuntTargetAlgo : public IBattleshipGameAlgo
{
public:
	HuntTargetAlgo();

	virtual ~HuntTargetAlgo() = default;

	void setBoard(int player, const char** board, int numRows, int numCols) override;

	bool init(const std::string& path) override;

	std::pair<int, int> attack() override;

	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;
};