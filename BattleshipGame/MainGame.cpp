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
#include <string>

using namespace battleship;
using std::unique_ptr;
using std::shared_ptr;
using std::map;
using std::exception;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

const int SUCCESS_CODE = 0;
const int ERROR_CODE = -1;
const int MAX_ARG_COUNT = 5;
const char* BP_CONFIG_PATH = "path";
const char* BP_CONFIG_DELAY = "-delay";
const char* BP_CONFIG_QUIET = "-quiet";
char* BP_CONFIG_FALSE = "false";
char* BP_CONFIG_TRUE = "true";

/** Fills the configuration dictionary with arguments that arrive from the command line,
 *	otherwise setting it with the defaults.
 */
bool parseArgs(int argc, char* argv[], map<const char*, char*>& config)
{
	if (argc > MAX_ARG_COUNT)
	{
		cerr << "Error: Too many arguments given. Try: BattleShipGame [path] [-delay <ms>] [-quiet]" << endl;
		return false;
	}

	config[BP_CONFIG_PATH] = ".";		// Nameless param, default is working directory
	config[BP_CONFIG_QUIET] = BP_CONFIG_FALSE;  // Default is print to screen
	config[BP_CONFIG_DELAY] = "1000";	// For animations, in ms. Default is a second.

	// If the first parameter doesn't match any of the configuration keywords this is a path
	if ((argc >= 2) && strcmp("-quiet", argv[1]) && strcmp("-delay", argv[1]))
	{
		config[BP_CONFIG_PATH] = argv[1];
	}

	// Digest rest of program arguments
	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], BP_CONFIG_DELAY))
		{
			if (argc > i + 1)
			{
				string argVal = argv[i + 1];
				if (IOUtil::isInteger(argVal))
				{
					config[BP_CONFIG_DELAY] = argv[i + 1];
				}
				else
				{
					cerr << "Error: Illegal delay field value. Try: -delay <ms>" << endl;
					return false;
				}
			}
			else
			{
				cerr << "Error: Delay argument missing value field. Try: -delay <ms>" << endl;
				return false;
			}
		}
		else if(!strcmp(argv[i], BP_CONFIG_QUIET))
		{
			config[BP_CONFIG_QUIET] = BP_CONFIG_TRUE;
		}
	}

	return true;
}

int main(int argc, char* argv[])
{
	try
	{
		// Define configuration and validate arguments
		map<const char*, char*> configuration;
		bool isLegalArgs = parseArgs(argc, argv, configuration);

		if (!isLegalArgs)
			return ERROR_CODE;

		// Load files
		auto inputFileNames = IOUtil::loadFilesInPath(string(configuration[BP_CONFIG_PATH]));
		if (NULL == inputFileNames)
			return ERROR_CODE;

		string boardFile = (*inputFileNames)[IOUtil::BOARD_SUFFIX];
		string playerAAttackFile = (*inputFileNames)[IOUtil::ATTACK_A_SUFFIX];
		string playerBAttackFile = (*inputFileNames)[IOUtil::ATTACK_B_SUFFIX];

		// Game initialization - players and board are initialized from files
		auto board = BattleshipGameBoardFactory::loadBattleBoard(BattleshipBoardInitTypeEnum::LOAD_BOARD_FROM_FILE,
																 boardFile);
		if (NULL == board)	// Invalid board setup
			return ERROR_CODE;

		GameFromFileAlgo playerA(playerAAttackFile);
		GameFromFileAlgo playerB(playerBAttackFile);

		unique_ptr<IGameVisual> visual = NULL;

		// Choose visualization type, depending on -quiet arg
		if (strcmp(configuration[BP_CONFIG_QUIET], BP_CONFIG_FALSE))
		{
			visual = std::make_unique<ConsoleMessageVisual>();
		}
		else
		{
			int delay = atoi(configuration[BP_CONFIG_DELAY]);
			visual = std::make_unique<TextualGuiVisual>(delay);
		}

		// Start a single game session, visualizer is expected to print the results to the screen when the session
		// is over (or during the session itself if this is a textual visualizer type)
		GameManager gameManager;
		gameManager.startGame(board, playerA, playerB, *visual);

		return SUCCESS_CODE;
	}
	catch (const exception& e)
	{	// This should be the last barrier that stops the app from failing
		cerr << "Error: General error of type " << e.what() << endl;
		return ERROR_CODE;
	}
}