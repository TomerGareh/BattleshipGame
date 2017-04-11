#include "MainGame.h"
#include "IBattleshipGameAlgo.h"
#include "BattleshipGameBoardFactory.h"
#include "GameFromFileAlgo.h"
#include "BattleBoard.h"
#include "GameManager.h"
#include "IGameVisual.h"
#include "ConsoleMessageVisual.h"
#include "TextualGuiVisual.h"
#include "IOUtil.h"

#include <cstdlib>
#include <memory>
#include <iostream>
#include <map>

using namespace battleship;
using std::unique_ptr;
using std::shared_ptr;
using std::map;

const char* BP_CONFIG_PATH = "path";
const char* BP_CONFIG_DELAY = "-delay";
const char* BP_CONFIG_QUIET = "-quiet";
char* BP_CONFIG_FALSE = "false";

void parseArgs(int argc, char* argv[], map<const char*, char*>& config)
{
	config[BP_CONFIG_PATH] = ".";		// Nameless param, default is working directory
	config[BP_CONFIG_QUIET] = BP_CONFIG_FALSE;  // Default is print to screen
	config[BP_CONFIG_DELAY] = "500";	// For animations, in ms. Default is half a second.

	if ((argc >= 2) && strcmp("-quiet", argv[1]) && strcmp("-delay", argv[1]))
	{
		config[BP_CONFIG_PATH] = argv[1];
	}

	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], BP_CONFIG_DELAY))
		{
			config[BP_CONFIG_DELAY] = argv[i];
		}
		else if(!strcmp(argv[i], BP_CONFIG_DELAY))
		{
			config[BP_CONFIG_DELAY] = argv[i];
		}
	}
}

int main(int argc, char* argv[])
{
	map<const char*, char*> configuration;
	parseArgs(argc, argv, configuration);

	auto inputFileNames = IOUtil::loadFilesInPath(string(configuration[BP_CONFIG_PATH]));
	if (NULL == inputFileNames)
		return -1; // TODO: Return some predefined error code

	string boardFile = (*inputFileNames)[IOUtil::BOARD_SUFFIX];
	string playerAAttackFile = (*inputFileNames)[IOUtil::ATTACK_A_SUFFIX];
	string playerBAttackFile = (*inputFileNames)[IOUtil::ATTACK_B_SUFFIX];

	// Game initialization
	GameFromFileAlgo playerA(playerAAttackFile);
	GameFromFileAlgo playerB(playerBAttackFile);

	auto board = BattleshipGameBoardFactory::loadBattleBoard(BattleshipBoardInitTypeEnum::LOAD_BOARD_FROM_FILE, boardFile);
	if (NULL == board)
		return -1; // TODO: Return some predefined error code

	unique_ptr<IGameVisual> visual = NULL;

	if (strcmp(configuration[BP_CONFIG_QUIET], BP_CONFIG_FALSE))
	{
		visual = std::make_unique<ConsoleMessageVisual>();
	}
	else
	{
		int delay = atoi(configuration[BP_CONFIG_DELAY]);
		visual = std::make_unique<TextualGuiVisual>(delay);
	}

	GameManager gameManager;
	gameManager.startGame(board, playerA, playerB, *visual);

	// TODO: Continue.. Print results do we need to do more..? Organize here according to instructions

	return 0;
}