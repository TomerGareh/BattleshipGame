#include "MainGame.h"
#include "BattleshipGameAlgoFactory.h"
#include "IBattleshipGameAlgo.h"
#include "BattleBoard.h"
#include "BoardBuilder.h"

#include <cstdlib>
#include <memory>
#include <iostream>

using namespace battleship;
using std::unique_ptr;
using std::shared_ptr;

shared_ptr<BattleBoard> buildBoardFromFile(char* path)
{
	BoardBuilder builder{};

	// TODO:
	// For each row/col in file in path - read from file and addPiece
	{
		int x = 0;
		int y = 0;
		char type = 'd';

		builder.addPiece(x, y, type);
	}

	auto board = builder.build();
	return board;
}

void loadFilesInPath(char* path)
{
	// system("dir /b /s /a-d * > file_names.txt");
}

int main(int argc, char* argv[])
{
	// TODO: load all 3 files here
	char* path = (argc > 1) ? argv[0] : ".";
	loadFilesInPath(path);

	// Game initialization
	unique_ptr<IBattleshipGameAlgo> algo = 
		BattleshipGameAlgoFactory::createGameAlgo(BattleshipGameAlgoTypeEnum::FILE_GAME_ALGO);

	auto board = buildBoardFromFile(path);
	if (NULL == board)
		return 2; // Some error code

	// algo->setBoard();

	// TODO: Continue.. Organize here

	return 0;
}