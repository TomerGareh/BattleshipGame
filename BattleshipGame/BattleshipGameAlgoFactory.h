#pragma once

#include <memory>
#include "IBattleshipGameAlgo.h"

using std::unique_ptr;

namespace battleship
{
	enum class BattleshipGameAlgoTypeEnum
	{
		FILE_GAME_ALGO
	};

	class BattleshipGameAlgoFactory
	{
	public:
		BattleshipGameAlgoFactory() = delete;
		~BattleshipGameAlgoFactory() = default;

		static unique_ptr<IBattleshipGameAlgo> createGameAlgo(BattleshipGameAlgoTypeEnum algoType);
	};
}