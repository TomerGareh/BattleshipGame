#include "BattleshipGameBoardFactory.h"
#include "BattleBoard.h"
#include "BoardBuilder.h"
#include "IOUtil.h"

namespace battleship
{
	// This function assumes that the file exists and the file name is valid
	shared_ptr<BattleBoard> BattleshipGameBoardFactory::buildBoardFromFile(const string& path)
	{
		shared_ptr<BoardBuilder> builder = std::make_shared<BoardBuilder>();
		shared_ptr<int> rowCounter = std::make_shared<int>(0);
		shared_ptr<int> colCounter = std::make_shared<int>(0);

		auto lineParser = [builder, rowCounter, colCounter](string& nextLine) mutable
		{
			// Read at the first BOARD_SIZE rows, ignore the rest
			if (*rowCounter >= BOARD_SIZE)
				return;

			*colCounter = 0;

			auto legalChars =
				{ 
					(char)toupper((char)BattleBoardSquare::Empty),
					(char)toupper((char)BattleBoardSquare::RubberBoat),
					(char)toupper((char)BattleBoardSquare::RocketShip),
					(char)toupper((char)BattleBoardSquare::Submarine),
					(char)toupper((char)BattleBoardSquare::Battleship),
					(char)tolower((char)BattleBoardSquare::RubberBoat),
					(char)tolower((char)BattleBoardSquare::RocketShip),
					(char)tolower((char)BattleBoardSquare::Submarine),
					(char)tolower((char)BattleBoardSquare::Battleship)
				};
			IOUtil::replaceIllegalCharacters(nextLine, (char)BattleBoardSquare::Empty, legalChars);

			// Traverse each character in the row and put into the board
			for (char& nextChar : nextLine)
			{
				builder->addPiece(*rowCounter, *colCounter, nextChar);
				(*colCounter)++;

				// Read at most BOARD_SIZE characters from each line, skip the rest
				if (*colCounter >= BOARD_SIZE)
					break;
			}

			// For rows missing characters - fill the rest with blanks
			while (*colCounter < BOARD_SIZE)
			{
				builder->addPiece(*rowCounter, *colCounter, (char)BattleBoardSquare::Empty);
				(*colCounter)++;
			}

			(*rowCounter)++;
		};

		// Start parsing the file, line by line
		IOUtil::parseFile(path, lineParser);

		// For missing rows - fill rows of blank squares
		while (*rowCounter < BOARD_SIZE)
		{
			for (*colCounter = 0; *colCounter < BOARD_SIZE; (*colCounter)++)
			{
				builder->addPiece(*rowCounter, *colCounter, (char)BattleBoardSquare::Empty);
			}

			(*rowCounter)++;
		}

		// Finalize the board, perform validation here
		auto board = builder->build();
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