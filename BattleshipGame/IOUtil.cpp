#include "IOUtil.h"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <locale>
#include <cerrno>
#include <sstream>

namespace battleship
{
	using std::ifstream;

	void IOUtil::replaceIllegalCharacters(string& line, const char replacementChar, initializer_list<char> legalChars)
	{
		// This lambda expression replaces all illegal characters with replacementChar
		// We compare to all legal possibilities and if we're not one of them -- replace.
		auto isIllegalChar = [legalChars](const char& c)
		{
			bool isLegal = false;

			for (const char& legalC : legalChars)
			{
				if (legalC == c)
				{
					isLegal = true;
					break;
				}
			}

			return isLegal;
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

	void IOUtil::parseFile(const string& filename, function<void(string& nextReadLine)> lineParser)
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

	void IOUtil::loadFilesInPath(const string& path)
	{
		const int BUFFER_SIZE = 1024;
		char systemCommand[BUFFER_SIZE];

		// Use the safer sprintf_s since Visual Studio throws warnings on sprintf due to potential misuse
		sprintf_s(systemCommand, BUFFER_SIZE, "2>NUL dir /a-d /b %s > filenames.txt", path);
		system(systemCommand);
	}
}