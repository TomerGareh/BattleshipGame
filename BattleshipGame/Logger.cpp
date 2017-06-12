#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

using std::cerr;
using std::endl;

namespace battleship
{
	const string Logger::LOG_FILE = "game.log";
		
	Logger::Logger()
	{
		fs.open(LOG_FILE, std::fstream::trunc | std::fstream::out | std::fstream::app);

		if (!fs.is_open() || !fs.good())
		{
			cerr << "Error: IO error when creating logger file: " << LOG_FILE << endl;
		}
	} 

	Logger::~Logger()
	{
		fs.close();

		// Stream errors are guaranteed to appear only after "flush",
		// which is only guaranteed when we explicitly flush or close the file for writing
		if (!fs)
		{
			cerr << "Error: IO error when flushing logger content to " << LOG_FILE << endl;
		}
	}

	Logger& Logger::getInstance()
	{
		// Guaranteed to be destroyed,
		// Instantiated on first use
		static Logger instance;

		return instance;
	}

	void Logger::log(Severity severity, const string& msg)
	{
		string severityStr;

		if (severity == Severity::ERROR_LEVEL)
			severityStr = "ERROR";
		else if (severity == Severity::WARNING_LEVEL)
			severityStr = "WARNING";
		else
			severityStr = "INFO";

		// Get current date-time
		time_t t = time(NULL);
		struct tm timeinfo;
		int rc = localtime_s(&timeinfo, &t);

		if (!rc)
			fs << "[" << std::put_time(&timeinfo, "%d-%m-%Y %H-%M-%S") << "][" << severityStr << "] " << msg << endl;
	}
}