#include "BattleshipGameAlgoFactory.h"
#include "GameFromFileAlgo.h"

namespace battleship
{
	#pragma region Section

	unique_ptr<IBattleshipGameAlgo> BattleshipGameAlgoFactory::createGameAlgo(BattleshipGameAlgoTypeEnum algoType)
	{
		switch (algoType)
		{
			case(BattleshipGameAlgoTypeEnum::FILE_GAME_ALGO):
			{
				return std::make_unique<IBattleshipGameAlgo>(GameFromFileAlgo());
			}

			default:
			{
				return NULL;
			}
		}
	}
}