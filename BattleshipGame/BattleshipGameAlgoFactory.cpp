#include "BattleshipGameAlgoFactory.h"

namespace battleship
{
	#pragma region Section

	unique_ptr<IBattleshipGameAlgo> createAlgoFromFile()
	{
		// TODO: fill this
		return NULL;
	}

	unique_ptr<IBattleshipGameAlgo> BattleshipGameAlgoFactory::createGameAlgo(BattleshipGameAlgoTypeEnum algoType)
	{
		switch (algoType)
		{
			case(BattleshipGameAlgoTypeEnum::FILE_GAME):
			{
				return createAlgoFromFile();
			}

			default:
			{
				return NULL;
			}
		}
	}
}