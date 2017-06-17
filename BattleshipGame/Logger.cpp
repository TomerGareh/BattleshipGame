#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

using std::cout;
using std::cerr;
using std::endl;
using std::lock_guard;

namespace battleship
{		
	Logger::Logger():
		_path(nullptr), // Default log level: show everything
		_limit(Severity::DEBUG_LEVEL)
	{
	} 

	Logger::~Logger()
	{
		log(Severity::INFO_LEVEL, "Terminating logger..");
		_fs.close();

		// Stream errors are guaranteed to appear only after "flush",
		// which is only guaranteed when we explicitly flush or close the file for writing
		if (!_fs)
		{
			auto logFilePath = *_path + "\\" + LOG_FILE;
			cerr << "Error: IO error when flushing logger content to " << logFilePath << endl;
		}
	}

	Logger& Logger::getInstance()
	{
		// Guaranteed to be destroyed,
		// Instantiated on first use
		static Logger instance;

		return instance;
	}

	void Logger::log(Severity severity, const string& msg, bool isPrintToConsole)
	{
		// Don't log messages before the logger is completely loaded
		if ((_path == nullptr) || (!_fs))
			return;

		// Filter messages that don't pass the bar of severity
		if (severity < _limit)
			return;

		string severityStr;

		if (severity == Severity::ERROR_LEVEL)
			severityStr = "ERROR";
		else if (severity == Severity::WARNING_LEVEL)
			severityStr = "WARNING";
		else if (severity == Severity::INFO_LEVEL)
			severityStr = "INFO";
		else
			severityStr = "DEBUG";

		{	// Keep output synchronized for multiple threads accessing it
			lock_guard<mutex> lock(_outputLock);

			// Get current date-time
			time_t t = time(nullptr);
			struct tm timeinfo;
			int rc = localtime_s(&timeinfo, &t);

			if (!rc)
				_fs << "[" << std::put_time(&timeinfo, "%d-%m-%Y %H-%M-%S") << "][" << severityStr << "] " << msg << endl;

			// Errors are force printed to console as well
			if (severity == Severity::ERROR_LEVEL)
			{
				cerr << msg << endl;
			}
			else if (isPrintToConsole)
			{
				cout << msg << endl;
			}
		}
	}

	Logger* Logger::setLevel(Severity limit)
	{
		_limit = limit;
		return this;
	}

	Logger* Logger::setPath(const string& path)
	{
		// Avoid incorrect usage
		if (_path)
		{
			return this;
		}

		_path = std::make_unique<string>(path);

		auto logFilePath = *_path + "\\" + LOG_FILE;

		// This should create the logger
		_fs.open(logFilePath, std::fstream::out | std::fstream::app);

		if (!_fs.is_open() || !_fs.good())
		{
			cerr << "Error: IO error when creating logger file at: " << logFilePath << endl <<
				    "[Game will proceed without logging]" << endl;
		}

		return this;
	}
}