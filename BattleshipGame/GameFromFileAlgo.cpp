#include <iostream>
#include <fstream>
#include <cstdio>
#include "GameFromFileAlgo.h"

GameFromFileAlgo::GameFromFileAlgo()
{
}

GameFromFileAlgo::~GameFromFileAlgo()
{
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



// This function assumes that the file exists and the file name is valid
void GameFromFileAlgo::readBoardFile(const std::string& fileName, char board[BOARD_SIZE][BOARD_SIZE])
{
	std::ifstream fin(fileName);
	char nextChar;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			fin.get(nextChar);
			if ((nextChar == '\n') || (nextChar == '\r\n') || (nextChar == EOF)) {
				break;
			}
			board[i][j] = nextChar;
		}
	}
}
