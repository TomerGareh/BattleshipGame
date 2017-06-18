#pragma once

#include "BattleshipGameBoardFactory.h"
#include "AlgoLoader.h"
#include "Configuration.h"

#include <memory>
#include <string>

using std::string;
using std::unique_ptr;
using std::shared_ptr;

namespace battleship
{
	/** Loads and runs the entire battleship competition game */
	class MainBattleshipGame
	{
	public:
		virtual ~MainBattleshipGame() = delete; // This class shouldn't be instantiated (or destroyed)

		/** Initiate the main game competition */
		static int run(int argc, char* argv[]);

		/** Success return code for app */
		static constexpr int SUCCESS_CODE = 0;

		/** Error return code for app */
		static constexpr int ERROR_CODE = -1;

	private:
		/** For logger param - to make it clear which messages are printed to the console */
		static constexpr bool PRINT_TO_CONSOLE = true;

		/** Hide the ctor - this class shouldn't be instantiated */
		MainBattleshipGame() = default;

		/** Begin the competition after all resources have been loaded and validated */
		static void startCompetition(const Configuration& config,
									 shared_ptr<BattleshipGameBoardFactory> boardFactory,
									 shared_ptr<AlgoLoader> algoLoader);

		/** Validate that there are enough valid loaded boards and algorithms */
		static bool validateLoadedResources(const Configuration& config,
											const vector<string>& loadedBoards,
											const vector<string>& loadedAlgos);

		/** Validates that there are enough existing board files and algo dll files */
		static bool validateAvailableResources(const Configuration& config,
											   shared_ptr<BattleshipGameBoardFactory> boardFactory,
											   shared_ptr<AlgoLoader> algoLoader);

		/** Validate the path defined in the configuration is accessible */
		static bool validatePath(const Configuration& config);

		/** Initialize the game logger */
		static void startLogger(const Configuration& config);
	};
}


