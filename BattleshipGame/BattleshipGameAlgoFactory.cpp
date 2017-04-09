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
		BoardBuilder builder;

		std::ifstream fin(path);
		char nextChar, nextCharUpper;
		bool isEOF;
		for (int i = 0; i < BOARD_SIZE; i++)
		{
			for (int j = 0; j < BOARD_SIZE; j++)
			{
				fin.get(nextChar);
				isEOF = fin.eof();
				if ((nextChar == '\n') || (nextChar == '\r\n') || isEOF)
				{
					break;
				}
				nextCharUpper = toupper(nextChar);
				if ((nextCharUpper != ' ') && (nextCharUpper != (char)ShipType::RubberBoat)
					&& (nextCharUpper != (char)ShipType::RocketShip) && (nextCharUpper != (char)ShipType::Submarine)
					&& (nextCharUpper != (char)ShipType::Battleship))
				{
					nextChar = ' ';
				}
				builder.addPiece(i, j, nextChar);
			}
			if (isEOF)
			{
				break;
			}
		}

		fin.close();

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