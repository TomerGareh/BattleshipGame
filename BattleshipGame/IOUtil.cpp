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

	const string IOUtil::BOARD_SUFFIX = ".sboard";
	const string IOUtil::ATTACK_A_SUFFIX = ".attack-a";
	const string IOUtil::ATTACK_B_SUFFIX = ".attack-b";

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

	void IOUtil::printLoadFileErrors(bool missingBoardFile, bool missingAttackAFile, bool missingAttackBFile,
									 const string& pathToDisplay)
	{
		if (missingBoardFile)
		{
			std::cout << "Missing board file (*.sboard) looking in path: " << pathToDisplay << std::endl;
		}

		if (missingAttackAFile)
		{
			std::cout << "Missing attack file for player A (*.attack-a) looking in path: " << pathToDisplay << std::endl;
		}

		if (missingAttackBFile)
		{
			std::cout << "Missing attack file for player B (*.attack-b) looking in path: " << pathToDisplay << std::endl;
		}
	}

	shared_ptr<map<string, string>> IOUtil::loadFilesInPath(const string& path)
	{
		const int BUFFER_SIZE = 1024;
		char systemCommand[BUFFER_SIZE];

		// Use the safer sprintf_s since Visual Studio throws warnings on sprintf due to potential misuse
		sprintf_s(systemCommand, BUFFER_SIZE, "2>NUL dir /a-d /b %s > filenames.txt", path);
		
		shared_ptr<map<string, string>> inputFileNames = std::make_shared<map<string, string>>();
		const string pathToDisplay = (path.compare(".") == 0) ? "Working Directory" : path;

		if (system(systemCommand) != 0)
		{
			bool missingBoardFile = true;
			bool missingAttackAFile = true;
			bool missingAttackBFile = true;

			auto lineParser = [missingBoardFile, missingAttackAFile, missingAttackBFile, inputFileNames](string& nextLine) mutable
			{
				if (missingBoardFile || missingAttackAFile || missingAttackBFile)
				{
					if (IOUtil::endsWith(nextLine, BOARD_SUFFIX))
					{
						missingBoardFile = false;
						(*inputFileNames)[BOARD_SUFFIX] = nextLine;
					}
					else if (IOUtil::endsWith(nextLine, ATTACK_A_SUFFIX))
					{
						missingAttackAFile = false;
						(*inputFileNames)[ATTACK_A_SUFFIX] = nextLine;
					}
					else if (IOUtil::endsWith(nextLine, ATTACK_B_SUFFIX))
					{
						missingAttackBFile = false;
						(*inputFileNames)[ATTACK_B_SUFFIX] = nextLine;
					}
				}
			};

			IOUtil::parseFile("filenames.txt", lineParser);

			if (missingBoardFile || missingAttackAFile || missingAttackBFile)
			{
				inputFileNames = NULL;
				IOUtil::printLoadFileErrors(missingBoardFile, missingAttackAFile, missingAttackBFile, pathToDisplay);
			}

			return inputFileNames;
		}
		else
		{
			std::cout << "Wrong path: " << pathToDisplay << std::endl;
			return NULL;
		}
	}
}