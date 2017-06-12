#include <iostream>
#include "BattleshipGameBoardFactory.h"
#include "IOUtil.h"
#include "Logger.h"

using std::cout;
using std::endl;

namespace battleship
{
	const string BattleshipGameBoardFactory::BOARD_SUFFIX = "sboard";

	bool BattleshipGameBoardFactory::parseHeader(string& nextLine, int& rows, int& cols, int& depth)
	{
		bool isValidFile = true;

		string delimiter = "x"; // Separates dimensions

		int dimensionIndex = 0;
		size_t pos = 0;
		string token;
		while ((pos = nextLine.find(delimiter)) != std::string::npos)
		{
			token = nextLine.substr(0, pos);
			nextLine.erase(0, pos + delimiter.length());

			// Parse each dimension
			if (IOUtil::isInteger(token))
			{
				int dimensionVal = stoi(token);

				switch (dimensionIndex)
				{
				case (0) : { cols = dimensionVal; break; }
				case (1) : { rows = dimensionVal; break; }
				case (2) : { depth = dimensionVal; break; }
				default: { break; }
				}

				dimensionIndex++;
			}
			else
			{
				isValidFile = false;
				break;
			}
		}
		
		// Parse the rest of the line (last dimension)
		if (IOUtil::isInteger(nextLine))
		{
			depth = stoi(nextLine);
			dimensionIndex++;
		}
		else
		{
			isValidFile = false;
		}

		isValidFile = isValidFile && (dimensionIndex == 3);

		return isValidFile;
	}

	void BattleshipGameBoardFactory::parseBoardRow(BoardBuilder& builder, string& nextLine,
												   int depthIndex, int rowIndex, int cols)
	{
		int colCounter = 0;

		auto legalChars =
		{
			(char)toupper((char)BattleBoardSquare::Empty),
			(char)toupper((char)BattleBoardSquare::RubberBoat),
			(char)toupper((char)BattleBoardSquare::RocketShip),
			(char)toupper((char)BattleBoardSquare::Submarine),
			(char)toupper((char)BattleBoardSquare::Battleship),
			(char)tolower((char)BattleBoardSquare::RubberBoat),
			(char)tolower((char)BattleBoardSquare::RocketShip),
			(char)tolower((char)BattleBoardSquare::Submarine),
			(char)tolower((char)BattleBoardSquare::Battleship)
		};
		IOUtil::replaceIllegalCharacters(nextLine, (char)BattleBoardSquare::Empty, legalChars);

		// Traverse each character in the row and put into the board
		for (char& nextChar : nextLine)
		{
			// Add to board only squares with real game pieces
			if (!(nextChar == static_cast<char>(BattleBoardSquare::Empty)))
				builder.addPiece(Coordinate(rowIndex, colCounter, depthIndex), nextChar);

			colCounter++;

			// Read at most "width" amount of cols characters from each line, skip the rest
			if (colCounter >= cols)
				break;
		}
	}

	shared_ptr<BattleBoard> BattleshipGameBoardFactory::buildBoardFromFile(const string& boardFile)
	{
		unique_ptr<BoardBuilder> builder;

		//Board dimensions
		int cols = 0;
		int rows = 0;
		int depth = 0;

		// Current position on board file
		int rowCounter = 0;
		int depthCounter = 0;

		auto headerParser = [&builder, &rows, &cols, &depth](string& nextLine, int lineNum,
															 bool& isHeader, bool& isValidFile)
		{
			if (lineNum == 1)
			{
				// Parse [cols]x[rows]x[depth] header
				isValidFile = parseHeader(nextLine, rows, cols, depth);
				builder = std::make_unique<BoardBuilder>(cols, rows, depth);
			}
			else if (lineNum == 2)
			{
				// Parse empty line - end of header
				isValidFile = nextLine.empty();
				isHeader = false; // End of header (2 lines only)
			}
		};

		auto lineParser = [&builder, &cols, &rows, &depth,
						   &rowCounter, &depthCounter](string& nextLine)
		{
			if (depthCounter >= depth)
			{
				return; // Read the first "depth" amount of levels, ignore the rest
			}
			else if (nextLine.empty())
			{
				rowCounter = 0; // Empty line - end of level data
				depthCounter++;
			}
			else if (rowCounter > rows)
			{
				return; // Read the first "height" amount of rows in each level, ignore the rest
			}
			else
			{	// Line with game pieces data - parse it
				parseBoardRow(*builder, nextLine, depthCounter, rowCounter, cols);
				rowCounter++;
			}
		};

		bool isValidFile = IOUtil::parseFile(boardFile, lineParser, headerParser);

		if (!isValidFile)
			return NULL;

		// Finalize the board, perform validation here
		auto board = builder->build();
		return board;
	}

	shared_ptr<BattleBoard> BattleshipGameBoardFactory::loadBattleBoard(const string& path)
	{
		auto boardFiles = IOUtil::listFilesInPath(path, BOARD_SUFFIX);

		if (boardFiles.size() == 0)
		{ // No board files found
			cout << "No board files (*.sboard) looking in path: " << path << endl;
			Logger::getInstance().log(Severity::ERROR_LEVEL,
									  "No board files (*.sboard) looking in path: " + path);
			return NULL;
		}

		// Choose the first board file found, lexicographically
		// Start parsing the file, line by line
		string boardFile = path + "\\" + boardFiles[0];

		return buildBoardFromFile(boardFile);
	}

	vector<shared_ptr<BattleBoard>> BattleshipGameBoardFactory::loadAllBattleBoards(const string& path)
	{
		vector<shared_ptr<BattleBoard>> loadedBoards;
		auto boardFiles = IOUtil::listFilesInPath(path, BOARD_SUFFIX);

		// Load each of the battle boards
		for (const auto& boardFilename : boardFiles)
		{
			Logger::getInstance().log(Severity::INFO_LEVEL, "Loading battle board: " + boardFilename + "..");
			string boardFile = path + "\\" + boardFilename;
			shared_ptr<BattleBoard> nextBoard = buildBoardFromFile(boardFile);

			// Accumulate only valid boards
			if (NULL != nextBoard)
			{
				loadedBoards.push_back(nextBoard);
				Logger::getInstance().log(Severity::INFO_LEVEL,
										  "Battle board " + boardFilename + " loaded successfully");
			}
		}

		// Compiler expected to perform return value optimization (vector is moved)
		return loadedBoards;
	}
}