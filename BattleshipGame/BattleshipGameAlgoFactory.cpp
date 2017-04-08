#include <iostream>
#include <fstream>
#include <cstdio>
#include <cctype>
#include "BattleshipGameAlgoFactory.h"
#include "IBattleshipGameAlgo.h"
#include "GameFromFileAlgo.h"
#include "BattleBoard.h"
#include "BoardBuilder.h"

namespace battleship
{
	#pragma region GameAlgo_Creation

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

	#pragma endregion
	#pragma region BattleBoard_Creation

	// This function assumes that the file exists and the file name is valid
	shared_ptr<BattleBoard> BattleshipGameAlgoFactory::buildBoardFromFile(const string& path)
	{
		BoardBuilder builder{};

		// TODO: Tomer - there is a bug below
		// See: http://www.cplusplus.com/forum/beginner/93910/
		std::ifstream fin(path);
		char nextChar;
		for (int i = 0; i < BOARD_SIZE; i++)
		{
			for (int j = 0; j < BOARD_SIZE; j++)
			{
				fin.get(nextChar);
				if ((nextChar == '\n') || (nextChar == '\r\n') || (nextChar == EOF))
				{
					break;
				}
				builder.addPiece(i, j, nextChar);
			}
		}

		auto board = builder.build();
		return board;
	}

	shared_ptr<BattleBoard> BattleshipGameAlgoFactory::loadBattleBoard(BattleshipBoardInitTypeEnum loadMethod,
																	   const string& param0)
	{
		switch (loadMethod)
		{
			case(BattleshipBoardInitTypeEnum::LOAD_BOARD_FROM_FILE):
			{
				return buildBoardFromFile(param0);
			}
			default:
			{
				return NULL;
			}
		}
	}

	shared_ptr<BattleBoard> BattleshipGameAlgoFactory::loadBattleBoard(BattleshipBoardInitTypeEnum loadMethod)
	{
		return loadBattleBoard(loadMethod, "");
	}

	#pragma endregion
}