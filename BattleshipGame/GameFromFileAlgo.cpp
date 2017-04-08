#include <memory>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cctype>
#include "GameFromFileAlgo.h"
#include "BoardBuilder.h"

namespace battleship
{
	GameFromFileAlgo::GameFromFileAlgo()
	{
	}

	GameFromFileAlgo::~GameFromFileAlgo()
	{
	}

	// This function assumes that the file exists and the file name is valid
	std::shared_ptr<BattleBoard> GameFromFileAlgo::buildBoardFromFile(const char* path)
	{
		BoardBuilder builder;

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

	void GameFromFileAlgo::setBoard(const char** board, int numRows, int numCols)
	{
	}

	std::pair<int, int> GameFromFileAlgo::attack()
	{
		return std::pair<int, int>();
	}

	void GameFromFileAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
	{
	}
}