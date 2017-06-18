#pragma once

#include <fstream>
#include <mutex>
#include <memory>

using std::fstream;
using std::string;
using std::mutex;
using std::unique_ptr;

namespace battleship
{
	enum class Severity : int
	{
		DEBUG_LEVEL = 0,
		INFO_LEVEL = 1,
		WARNING_LEVEL = 2,
		ERROR_LEVEL = 3
	};

	/** Thread safe singelton logger class.
	 *	Logger is usable only after setPath() have been called and a log file have been created.
	 */
	class Logger
	{
	public:
		virtual ~Logger();

		/** Gets single instance of logger */
		static Logger& getInstance();

		// Delete copy & move constructors
		Logger(Logger const&) = delete;
		void operator=(Logger const&) = delete;
		Logger(Logger&& other) noexcept = delete;
		Logger& operator= (Logger&& other) = delete;

		static string severityToString(Severity severity);

		/** Logs a single message to log file */
		void log(Severity severity, const string& msg, bool isPrintToConsole = false);

		/** Set level of filtering messages for the logger.
		 *  Messages with a lower severity than limit won't be logged.
		 */
		Logger* setLevel(Severity limit);

		/** Sets a path for the log file and creates it.
		 *	The logger is usable only after this method is called.
		 *  Repeated calls to this method do nothing.
		 */
		Logger* setPath(const string& path);

	private:
		static constexpr auto LOG_FILE = "game.log"; // Log file name
		unique_ptr<string> _path; // Path of the log file, logger is active only after this is initialized

		fstream _fs; // Logger file pointer
		Severity _limit; // Limit of which log messages are filtered
		
		mutex _outputLock; // Keeps output synchronized

		Logger(); // Don't allow instantiation from outside
	};
}


