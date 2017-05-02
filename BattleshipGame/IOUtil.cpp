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
		// TODO: Tomer - implement
		return false;
	}

	vector<string> IOUtil::listFilesInPath(const string& path, const string& extension)
	{

		// TODO: Tomer - rewrite without system
		/*
		const int BUFFER_SIZE = 1024;
		char systemCommand[BUFFER_SIZE];
		string pathWithQuotes = '\"' + path + '\"';

		// Use the safer sprintf_s since Visual Studio throws warnings on sprintf due to potential misuse
		sprintf_s(systemCommand, BUFFER_SIZE, "2>NUL dir /a-d /b %s > filenames.txt", pathWithQuotes.c_str());
		
		shared_ptr<map<string, string>> inputFileNames = std::make_shared<map<string, string>>();
		const string pathToDisplay = (path.compare(".") == 0) ? "Working Directory" : path;

		if (system(systemCommand) == 0)
		{
			shared_ptr<bool> missingBoardFile = std::make_shared<bool>(true);
			shared_ptr<bool> missingAttackAFile = std::make_shared<bool>(true);
			shared_ptr<bool> missingAttackBFile = std::make_shared<bool>(true);

			auto lineParser = [missingBoardFile, missingAttackAFile, missingAttackBFile,
							   inputFileNames, path](string& nextLine) mutable
			{
				if ((*missingBoardFile) || (*missingAttackAFile) || (*missingAttackBFile))
				{
					if (IOUtil::endsWith(nextLine, BOARD_SUFFIX))
					{
						*missingBoardFile = false;
						(*inputFileNames)[BOARD_SUFFIX] = path + '/' + nextLine;
					}
					else if (IOUtil::endsWith(nextLine, ATTACK_A_SUFFIX))
					{
						*missingAttackAFile = false;
						(*inputFileNames)[ATTACK_A_SUFFIX] = path + '/' + nextLine;
					}
					else if (IOUtil::endsWith(nextLine, ATTACK_B_SUFFIX))
					{
						*missingAttackBFile = false;
						(*inputFileNames)[ATTACK_B_SUFFIX] = path + '/' + nextLine;
					}
				}
			};

			IOUtil::parseFile("filenames.txt", lineParser);

			if ((*missingBoardFile) || (*missingAttackAFile) || (*missingAttackBFile))
			{
				inputFileNames = NULL;
				IOUtil::printLoadFileErrors(*missingBoardFile, *missingAttackAFile, *missingAttackBFile, pathToDisplay);
			}

			return inputFileNames;
		}
		else
		{
			std::cout << "Wrong path: " << pathToDisplay << std::endl;
			return NULL;
		}

		*/

		vector<string> v;
		return v;
	}

	string IOUtil::convertPathToAbsolute(const string& path)
	{
		// TODO: Tomer - implement
		return "";

		// TODO: Verify RVO happens here for string (it should)
	}
}