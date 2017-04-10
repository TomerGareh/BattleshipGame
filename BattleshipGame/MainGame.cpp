#include "MainGame.h"
#include "IBattleshipGameAlgo.h"
#include "BattleshipGameBoardFactory.h"
#include "GameFromFileAlgo.h"
#include "BattleBoard.h"
#include "GameManager.h"
#include "IGameVisual.h"
#include "ConsoleMessageVisual.h"
#include "IOUtil.h"

#include <cstdlib>
#include <memory>
#include <iostream>

using namespace battleship;
using std::unique_ptr;
using std::shared_ptr;

int main(int argc, char* argv[])
{
	// TODO: load all 3 files here
	char* path = (argc > 1) ? argv[0] : ".";
	IOUtil::loadFilesInPath(path);
	string playerAAttackFile;
	string playerBAttackFile;
	// ...

	// Game initialization
	GameFromFileAlgo playerA(playerAAttackFile);
	GameFromFileAlgo playerB(playerBAttackFile);

	auto board = BattleshipGameBoardFactory::loadBattleBoard(BattleshipBoardInitTypeEnum::LOAD_BOARD_FROM_FILE);
	if (NULL == board)
		return -1; // TODO: Return some predefined error code

	ConsoleMessageVisual visualization;
	GameManager gameManager;
	gameManager.startGame(board, playerA, playerB, visualization);

	// TODO: Continue.. Print results.. Organize here

	return 0;
}