#pragma once

#include <string>
#include <vector>
#include <utility>
#include "Logger.h"

using std::string;
using std::pair;
using std::vector;

namespace battleship
{
	/** Loads and stores configuration for battleship game */
	class Configuration
	{
	public:
		/** Loads configuration in the following priority:
		 *  1) Load configuration file
		 *  2) Use defaults if configuration file is not available
		 */
		Configuration();
		virtual ~Configuration() = default;

		// Path to load algorithms and boards from
		string path;

		// Number of worker threads to run games in competition
		int threads;

		// List of textual warnings (if any) for incorrect configuration setup.
		// The configuration object accumulates these since nothing is loaded in the app yet,
		// including the logger.
		vector<pair<Severity, string>> configurationIssues;

		/** Parses the arguements in the command line and overrides the configuration accordingly.
	 	 *  Returns true if successful or false if the command line was illegal.
		 */
		bool parseArgs(int argc, char* argv[]);

	private:
		// Configuration file name
		static constexpr auto CONFIG_FILE = "config.ini";

		// Default working dir path of dlls / boards
		static constexpr auto DEFAULT_PATH = ".";

		// Default amount of worker threads
		static constexpr int DEFAULT_THREAD_COUNT = 4;

		// Maximum number of arguments in a legal command line
		static constexpr int MAX_ARG_COUNT = 4;

		// Header of threads arg in command line
		static constexpr char* BP_CONFIG_THREADS = "-threads";

		// Header of dlls / boards path arg in configuration file
		static constexpr auto CONFIG_HEADER_PATH = "PATH=";

		// Header of threads arg in configuration file
		static constexpr auto CONFIG_HEADER_THREADS = "THREADS=";

		// Beginning of comments in config file - to be ignored by the parser
		static constexpr auto CONFIG_HEADER_COMMENT = "%%";

		// The character that may optionally surround values in the config file ("value")
		static constexpr auto CONFIG_VALUE_MARKER = "\"";

		/** Parses the configuration file and sets the configuration accordingly.
		 *  Returns true if configuration file loaded successfuly or false if not.
		 */
		bool loadConfigFile();

		/** Loads default values for configuration (last fallback) */
		void loadDefaults();

		/** Normalizes the value given: if it starts and ends with quotation marks they will be removed */
		static void normalizeValue(string& value);
	};
}