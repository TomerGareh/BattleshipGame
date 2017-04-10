#include <fstream>
#include "BattleshipGameBoardFactory.h"
#include "BattleBoard.h"
#include "BoardBuilder.h"
#include "IOUtil.h"

namespace battleship
{
	// This function assumes that the file exists and the file name is valid
	shared_ptr<BattleBoard> BattleshipGameBoardFactory::buildBoardFromFile(const string& path)
	{
		BoardBuilder builder;

		// TODO: Tomer - use IOUtil
		// See example at GameFromFileAlgo - populateFromFile
		/* -----------*/
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
		/* -----------*/

		auto board = builder.build();
		return board;
	}

	shared_ptr<BattleBoard> BattleshipGameBoardFactory::loadBattleBoard(BattleshipBoardInitTypeEnum loadMethod,
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

	shared_ptr<BattleBoard> BattleshipGameBoardFactory::loadBattleBoard(BattleshipBoardInitTypeEnum loadMethod)
	{
		return loadBattleBoard(loadMethod, "");
	}
}