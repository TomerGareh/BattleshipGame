#include <iostream>
#include "BattleshipGameBoardFactory.h"
#include "IOUtil.h"
#include "Logger.h"
#include "BoardBuilder.h"

using std::cout;
using std::endl;

namespace battleship
{
	const string BattleshipGameBoardFactory::BOARD_SUFFIX = "sboard";

	BattleshipGameBoardFactory::BattleshipGameBoardFactory(const string& path): _path(path)
	{
		Logger::getInstance().log(Severity::DEBUG_LEVEL, "BattleshipGameBoardFactory started..");
		_availableBoards = IOUtil::listFilesInPath(path, BOARD_SUFFIX);
	}

	bool BattleshipGameBoardFactory::parseHeader(string& nextLine, int& rows, int& cols, int& depth) const
	{
		bool isValidFile = true;

		string delimiter = "x"; // Separates dimensions

		int dimensionIndex = 0;
		size_t pos;
		while ((pos = nextLine.find(delimiter)) != std::string::npos)
		{
			string token = nextLine.substr(0, pos);
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
			static_cast<char>(toupper(static_cast<char>(BattleBoardSquare::Empty))),
			static_cast<char>(toupper(static_cast<char>(BattleBoardSquare::RubberBoat))),
			static_cast<char>(toupper(static_cast<char>(BattleBoardSquare::RocketShip))),
			static_cast<char>(toupper(static_cast<char>(BattleBoardSquare::Submarine))),
			static_cast<char>(toupper(static_cast<char>(BattleBoardSquare::Battleship))),
			static_cast<char>(tolower(static_cast<char>(BattleBoardSquare::RubberBoat))),
			static_cast<char>(tolower(static_cast<char>(BattleBoardSquare::RocketShip))),
			static_cast<char>(tolower(static_cast<char>(BattleBoardSquare::Submarine))),
			static_cast<char>(tolower(static_cast<char>(BattleBoardSquare::Battleship)))
		};
		IOUtil::replaceIllegalCharacters(nextLine, static_cast<char>(BattleBoardSquare::Empty), legalChars);

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

	unique_ptr<BattleBoard> BattleshipGameBoardFactory::buildBoardFromFile(const string& boardFile)
	{
		unique_ptr<BoardBuilder> builder;

		//Board dimensions
		int cols = 0;
		int rows = 0;
		int depth = 0;

		// Current position on board file
		int rowCounter = 0;
		int depthCounter = 0;

		auto headerParser = [this, &builder, &rows, &cols, &depth](string& nextLine, int lineNum,
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

		auto lineParser = [this, &builder, &cols, &rows, &depth,
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
			return nullptr;

		// Finalize the board, perform validation here
		auto board = builder->build();
		return board;
	}

	const vector<string>& BattleshipGameBoardFactory::loadAllBattleBoards()
	{
		// Load each of the battle boards
		for (const auto& boardFilename : _availableBoards)
		{
			Logger::getInstance().log(Severity::INFO_LEVEL, "Loading battle board: " + boardFilename + "..");
			string boardFile = _path + "\\" + boardFilename;
			unique_ptr<BattleBoard> nextBoard = buildBoardFromFile(boardFile);

			// Accumulate only valid boards
			if (nullptr != nextBoard)
			{
				_loadedBoards.emplace(make_pair(boardFilename, std::move(nextBoard)));
				_loadedBoardNames.push_back(boardFilename);
				Logger::getInstance().log(Severity::INFO_LEVEL,
										  "Battle board " + boardFilename + " loaded successfully");
			}
			else
			{
				Logger::getInstance().log(Severity::WARNING_LEVEL,
										  "Battle board " + boardFilename + " is invalid");
			}
		}

		return _loadedBoardNames;
	}

	shared_ptr<BattleBoard> BattleshipGameBoardFactory::requestBattleboard(const string& path)
	{
		auto boardIt = _loadedBoards.find(path);

		if (boardIt == _loadedBoards.end())
		{
			return nullptr;
		}
		else
		{
			Logger::getInstance().log(Severity::DEBUG_LEVEL, path + " BattleBoard new instance created..");
			return BoardBuilder::clone(*boardIt->second); // Prototype pattern
		}
	}

	const vector<string>& BattleshipGameBoardFactory::availableBoardsList() const
	{
		return _availableBoards;
	}

	const vector<string>& BattleshipGameBoardFactory::loadedBoardsList() const
	{
		return _loadedBoardNames;
	}
}