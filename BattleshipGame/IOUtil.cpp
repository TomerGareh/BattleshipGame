#include "IOUtil.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <locale>
#include <cerrno>
#include <sstream>

namespace battleship
{
	using std::ifstream;

	IOUtil::IOUtil()
	{
	}


	IOUtil::~IOUtil()
	{
	}

	void IOUtil::replaceIllegalCharacters(string& line, const char replacementChar)
	{
		// This lambda expression replaces all non digit and punctuation separator characters with replacementChar
		auto isIllegalChar = [](const char& c)
		{
			return (!((c >= '0') && (c <= '9')) && (c != COORDS_DELIM));
		};

		std::replace_if(line.begin(), line.end(), isIllegalChar, replacementChar);
	}

	void IOUtil::removeLeadingTrailingSpaces(string& line)
	{
		// Remove all leading, trailing and in-between spaces
		std::string::iterator end_pos = std::remove(line.begin(), line.end(), ' ');
		line.erase(end_pos, line.end());
		end_pos = std::remove(line.begin(), line.end(), '\t');
		line.erase(end_pos, line.end());
	}

	void IOUtil::parseFile(string filename, function<void(string& nextReadLine)> lineParser)
	{
		string nextLine;
		ifstream fs(filename);

		if (!fs.is_open())
			std::cerr << "Error: Failed to open file " << filename << std::endl;

		// Read the next line from the file, in case of an error this is skipped
		while (getline(fs, nextLine))
		{
			// Get rid of \r\n line endings
			if (nextLine.size() && nextLine[nextLine.size() - 1] == '\r')
			{
				nextLine = nextLine.substr(0, nextLine.size() - 1);
			}

			// Invoke parser on next line read
			lineParser(nextLine);
		}

		// This term is activated only in the case when ifstream's badbit is set
		if (fs.bad())
			std::cerr << "Error: IO error occured while reading file " << filename << std::endl;

		fs.close();
	}
}