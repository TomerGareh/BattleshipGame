#include "MainGame.h"
#include "BattleshipGameBoardFactory.h"
#include "IOUtil.h"
#include "AlgoLoader.h"
#include "Logger.h"
#include "Configuration.h"
#include "CompetitionManager.h"

#include <memory>
#include <iostream>
#include <map>

using namespace battleship;
using std::unique_ptr;
using std::shared_ptr;
using std::map;
using std::exception;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::to_string;

const int SUCCESS_CODE = 0;
const int ERROR_CODE = -1;

int main(int argc, char* argv[])
{
	try
	{
		// #1 - Load configuration
		Configuration config;
		bool isLegalArgs = config.parseArgs(argc, argv);

		if (!isLegalArgs) // Error already printed by configuration parser
			return ERROR_CODE;

		// #2 - Start logger
		Logger::getInstance().setPath(config.path)->setLevel(Severity::INFO_LEVEL);
		Logger::getInstance().log(Severity::INFO_LEVEL, "Battleship game started.");
		Logger::getInstance().log(Severity::INFO_LEVEL, "Path = " + config.path);
		Logger::getInstance().log(Severity::INFO_LEVEL, "Worker threads count = " + to_string(config.threads));
		bool isPrintToConsole = true; // Specifier for messages that go to the console

		// #3 - Load and build game resources: board & algorithms

		// Validation #1: Invalid resources path
		if (!IOUtil::validatePath(config.path))
		{
			Logger::getInstance().log(Severity::ERROR_LEVEL, "Wrong path: " + config.path, isPrintToConsole);
			Logger::getInstance().log(Severity::INFO_LEVEL, "Battleship game ended.");
			return ERROR_CODE;
		}

		// Game initialization - load board and player algorithms
		const string absolutePath = IOUtil::convertPathToAbsolute(config.path);
		auto boardFactory = std::make_shared<BattleshipGameBoardFactory>(absolutePath);
		auto validBoards = boardFactory->boardsList();

		if (validBoards.empty())
		{
			Logger::getInstance().log(Severity::ERROR_LEVEL,
									  "No board files(*.sboard) looking in path: " + config.path,
								      isPrintToConsole);
		}

		auto algoLoader = std::make_shared<AlgoLoader>(absolutePath);
		auto availableAlgos = algoLoader->availableGameAlgos();

		if (availableAlgos.size() < 2)
		{ // TODO: Is this absolutePath or config.path..???
			Logger::getInstance().log(Severity::ERROR_LEVEL,
									  "Missing an algorithm (dll) file looking in path: " + absolutePath,
									  isPrintToConsole);
		}

		// Validation #2: Missing board path, Invalid board setup, Missing dll files
		if (validBoards.empty() || availableAlgos.size() < 2)
		{
			Logger::getInstance().log(Severity::INFO_LEVEL, "Battleship game ended.");
			return ERROR_CODE;
		}

		Logger::getInstance().log(Severity::INFO_LEVEL,
								  "Number of legal players: " + algoLoader->loadedGameAlgos().size(),
								  isPrintToConsole);
		Logger::getInstance().log(Severity::INFO_LEVEL,
								  "Number of legal boards: " + validBoards.size(),
								  isPrintToConsole);

		Logger::getInstance().log(Severity::DEBUG_LEVEL, "All resources validated, proceeding to competition");
		CompetitionManager competitionMgr(boardFactory, algoLoader, config.threads);

		Logger::getInstance().log(Severity::DEBUG_LEVEL, "Competition tasks ready to run..");
		competitionMgr.run();

		Logger::getInstance().log(Severity::INFO_LEVEL, "Battleship game ended.");

		return SUCCESS_CODE;
	}
	catch (const exception& e)
	{	// This should be the last barrier that stops the app from failing
		cerr << "Error: General error of type " << e.what() << endl;
		return ERROR_CODE;
	}
}