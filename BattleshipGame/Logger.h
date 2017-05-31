#pragma once

#include <fstream>
#include <string>

using std::fstream;
using std::string;

namespace battleship
{
	enum class Severity : char
	{
		INFO_LEVEL,
		WARNING_LEVEL,
		ERROR_LEVEL
	};

	/** Thread safe singelton logger class */
	class Logger
	{
	public:
		virtual ~Logger();

		static Logger& getInstance();

		// Delete copy & move constructors
		Logger(Logger const&) = delete;
		void operator=(Logger const&) = delete;
		Logger(Logger&& other) noexcept = delete;
		Logger& operator= (Logger&& other) = delete;

		void log(Severity severity, const string& msg);

	private:
		Logger(); // Don't allow instantiation
		fstream fs; // Logger file pointer

		static const string LOG_FILE; // Log file name
	};
}


