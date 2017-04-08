#include "MainGame.h"
#include "IBattleshipGameAlgo.h"
#include "BattleshipGameAlgoFactory.h"
#include "GameFromFileAlgo.h"
#include "BattleBoard.h"
#include "BoardBuilder.h"

#include <cstdlib>
#include <memory>
#include <iostream>

using namespace battleship;
using std::unique_ptr;
using std::shared_ptr;

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


	auto board = (static_cast<unique_ptr<GameFromFileAlgo>>(algo))->buildBoardFromFile(path);
	if (NULL == board)
		return 2; // Some error code

	// algo->setBoard();

	// TODO: Continue.. Organize here

	return 0;
}