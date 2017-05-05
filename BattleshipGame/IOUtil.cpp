#include "IOUtil.h"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <locale>
#include <cerrno>
#include <sstream>
#include <windows.h>

namespace battleship
{
	using std::ifstream;

	bool IOUtil::isInteger(const std::string & s)
	{
		if (s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+')))
		{
			return false;
		}

		char * p;
		strtol(s.c_str(), &p, 10);

		return (*p == 0);
	}

	void IOUtil::replaceIllegalCharacters(string& line, const char replacementChar, initializer_list<char> legalChars)
	{
		// This lambda expression replaces all illegal characters with replacementChar
		// We compare to all legal possibilities and if we're not one of them -- replace.
		auto isIllegalChar = [legalChars](const char& c)
		{
			bool isIllegal = true;

			for (const char& legalC : legalChars)
			{
				if (legalC == c)
				{
					isIllegal = false;
					break;
				}
			}

			return isIllegal;
		};

		std::replace_if(line.begin(), line.end(), isIllegalChar, replacementChar);
	}

	void IOUtil::removeLeadingTrailingSpaces(string& line)
	{
		// Remove all leading and trailing spaces
		size_t first = line.find_first_not_of("\t ");
		first = (first == string::npos) ? 1 : first;
		size_t last = line.find_last_not_of("\t ");
		last = (last == string::npos) ? 1 : (last + 1);
		line = line.substr(first, (last - first));
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

	bool IOUtil::endsWith(const string& fullString, const string& ending)
	{
		if (fullString.length() >= ending.length())
		{
			return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
		}
		else
		{
			return false;
		}
	}

	bool IOUtil::validatePath(const string& path)
	{
		WIN32_FIND_DATAA fileData;
		HANDLE dir = FindFirstFileA((path + "\\*").c_str(), &fileData);	// Notice: Unicode compatible version of FindFirstFile	
		bool isValid = (dir != INVALID_HANDLE_VALUE);
		FindClose(dir);
		return isValid;
	}

	vector<string> IOUtil::listFilesInPath(const string& path, const string& extension)
	{
		vector<string> fileList;
		WIN32_FIND_DATAA fileData;
		HANDLE dir = FindFirstFileA((path + "\\*." + extension).c_str(), &fileData);	// Notice: Unicode compatible version of FindFirstFile
		if (dir != INVALID_HANDLE_VALUE)	// We assume that the path is valid here, this shouldn't happen
		{
			do
			{
				string fileName = path + "\\" + fileData.cFileName;	
				fileList.push_back(fileName);
			} while (FindNextFileA(dir, &fileData));	// Notice: Unicode compatible version of FindNextFile
		}

		FindClose(dir);
		
		std::sort(fileList.begin(), fileList.end());

		return fileList;
	}

	string IOUtil::convertPathToAbsolute(const string& path)
	{
		const int BUFFER_SIZE = 1024;
		char buffer[BUFFER_SIZE];
		int pathSize = GetFullPathNameA(path.c_str(), BUFFER_SIZE, buffer, NULL);
		string fullPath(buffer, pathSize);
		return fullPath;
		// TODO: Verify RVO happens here for string (it should)
	}
}