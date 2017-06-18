#include "MainBattleshipGame.h"

#include "IOUtil.h"
#include "Logger.h"
#include "CompetitionManager.h"
#include <iostream>

using std::exception;
using std::cout;
using std::cerr;
using std::endl;
using std::to_string;

namespace battleship
{
	void MainBattleshipGame::startCompetition(const Configuration& config,
											  shared_ptr<BattleshipGameBoardFactory> boardFactory,
											  shared_ptr<AlgoLoader> algoLoader)
	{
		Logger::getInstance().log(Severity::INFO_LEVEL,
			"Number of legal players: " +
			to_string(static_cast<unsigned int>(algoLoader->loadedGameAlgos().size())),
			PRINT_TO_CONSOLE);
		Logger::getInstance().log(Severity::INFO_LEVEL,
			"Number of legal boards: " +
			to_string(static_cast<unsigned int>(boardFactory->loadedBoardsList().size())),
			PRINT_TO_CONSOLE);

		Logger::getInstance().log(Severity::DEBUG_LEVEL, "All resources validated, proceeding to competition");
		CompetitionManager competitionMgr(boardFactory, algoLoader, config.threads);

		Logger::getInstance().log(Severity::DEBUG_LEVEL, "Competition tasks ready to run..");
		competitionMgr.run();

		Logger::getInstance().log(Severity::INFO_LEVEL, "Battleship game ended.");
	}

	bool MainBattleshipGame::validateLoadedResources(const Configuration& config,
												     const vector<string>& loadedBoards,
												     const vector<string>& loadedAlgos)
	{
		if (loadedBoards.empty())
		{
			Logger::getInstance().log(Severity::ERROR_LEVEL,
				"No valid board files (*.sboard) looking in path: " + config.path,
				PRINT_TO_CONSOLE);
		}

		if (loadedAlgos.size() < 2)
		{ // TODO: Is this absolutePath or config.path..???
			Logger::getInstance().log(Severity::ERROR_LEVEL,
				"Missing valid algorithms looking in path: " + config.path + " (need at least two)",
				PRINT_TO_CONSOLE);
		}

		if (loadedBoards.empty() || loadedAlgos.size() < 2)
		{
			Logger::getInstance().log(Severity::INFO_LEVEL, "Battleship game ended.");
			return false;
		}

		return true;
	}

	bool MainBattleshipGame::validateAvailableResources(const Configuration& config,
													    shared_ptr<BattleshipGameBoardFactory> boardFactory,
														shared_ptr<AlgoLoader> algoLoader)
	{
		auto availableBoards = boardFactory->availableBoardsList();

		if (availableBoards.empty())
		{
			Logger::getInstance().log(Severity::ERROR_LEVEL,
				"No board files (*.sboard) looking in path: " + config.path,
				PRINT_TO_CONSOLE);
		}

		auto availableAlgos = algoLoader->availableGameAlgos();

		if (availableAlgos.size() < 2)
		{ // TODO: Is this absolutePath or config.path..???
			auto absolutePath = IOUtil::convertPathToAbsolute(config.path);
			Logger::getInstance().log(Severity::ERROR_LEVEL,
				"Missing an algorithm (dll) file looking in path: " + absolutePath + " (need at least two)",
				PRINT_TO_CONSOLE);
		}

		if (availableBoards.empty() || availableAlgos.size() < 2)
		{
			Logger::getInstance().log(Severity::INFO_LEVEL, "Battleship game ended.");
			return false;
		}

		return true;
	}

	bool MainBattleshipGame::validatePath(const Configuration& config)
	{
		if (!IOUtil::validatePath(config.path))
		{
			Logger::getInstance().log(Severity::ERROR_LEVEL, "Wrong path: " + config.path, PRINT_TO_CONSOLE);
			Logger::getInstance().log(Severity::INFO_LEVEL, "Battleship game ended.");
			return false;
		}

		return true;
	}

	void MainBattleshipGame::startLogger(const Configuration& config)
	{
		Logger::getInstance().setPath(config.path)->setLevel(Severity::INFO_LEVEL);
		Logger::getInstance().log(Severity::INFO_LEVEL, "Battleship game started.");
		Logger::getInstance().log(Severity::INFO_LEVEL, "Path = " + config.path);
		Logger::getInstance().log(Severity::INFO_LEVEL, "Worker threads count = " + to_string(config.threads));
	}

	int MainBattleshipGame::run(int argc, char* argv[])
	{
		try
		{
			// #1 - Load configuration
			Configuration config;
			bool isLegalArgs = config.parseArgs(argc, argv);

			if (!isLegalArgs) // Error already printed by configuration parser
				return ERROR_CODE;

			// #2 - Start logger
			startLogger(config);

			// #3 - Load and build game resources: board & algorithms

			// Validation #1: Invalid resources path
			if (!validatePath(config))
				return ERROR_CODE;

			// Game initialization - load board and player algorithms
			const string absolutePath = IOUtil::convertPathToAbsolute(config.path);
			auto boardFactory = std::make_shared<BattleshipGameBoardFactory>(absolutePath);
			auto algoLoader = std::make_shared<AlgoLoader>(absolutePath);

			// Validation #2: Missing boards, Missing dll files
			if (!validateAvailableResources(config, boardFactory, algoLoader))
				return ERROR_CODE;

			auto loadedBoards = boardFactory->loadAllBattleBoards();
			auto loadedAlgos = algoLoader->loadAllAvailableAlgorithms();

			// Validation #3: Not enough valid boards or dlls
			if (!validateLoadedResources(config, loadedBoards, loadedAlgos))
				return ERROR_CODE;

			// All validations complete - begin battleship competition between resources
			startCompetition(config, boardFactory, algoLoader);

			return SUCCESS_CODE;
		}
		catch (const exception& e)
		{	// Protect entire game from failing.
			// Errors that are caught by this barrier are logged with the logger
			string errorMsg = e.what();
			Logger::getInstance().log(Severity::ERROR_LEVEL,
									  "Error: General error of type " + errorMsg, PRINT_TO_CONSOLE);
			return ERROR_CODE;
		}
	}
}