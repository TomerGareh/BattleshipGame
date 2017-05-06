#include "GameFromFileAlgo.h"
#include "IOUtil.h"
#include "BattleBoard.h"
#include "AlgoCommon.h"
#include <exception>
#include <iostream>

using namespace battleship;
using std::exception;
using std::cerr;
using std::endl;

const string GameFromFileAlgo::ATTACK_SUFFIX = "attack";

void GameFromFileAlgo::populateMovesFromFile(const string& filename)
{
	// This line-parser logic will clean the line string from illegal characters and spaces,
	// and then attempt to break it to tokens and look for a pair of numbers.
	// If a pair of legal coordinates is encountered, we add it to the move list.
	// We pass a local ref to the object's field: _predefinedMoves, because this single field can't
	// be captured inside the lambda expression (we have to capture "this" if we want to allow access,
	// instead we capture a "fake" reference to the _predefinedMoves field)
	auto& moveListPtr = _predefinedMoves;
	int rows = _numRows;
	int cols = _numCols;

	auto lineParser = [&moveListPtr, rows, cols](string& nextLine)
	{
		auto legalChars = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', COORDS_DELIM };
		IOUtil::replaceIllegalCharacters(nextLine, (char)BoardSquare::Empty, legalChars);

		// Search for punctuation delimeter, if not found this is an illegal row, skip it (do nothing)
		size_t delim_pos;
		if ((delim_pos = nextLine.find(GameFromFileAlgo::COORDS_DELIM)) != string::npos)
		{
			// Break to tokens
			string firstToken = nextLine.substr(0, delim_pos);
			nextLine.erase(0, delim_pos + 1);
			string& secondToken = nextLine;

			if ((!firstToken.empty()) && (!secondToken.empty()))
			{
				IOUtil::removeLeadingTrailingSpaces(firstToken);
				IOUtil::removeLeadingTrailingSpaces(secondToken);

				// Deal with whitespaces and other characters after the second number
				size_t secondTokenLastPos = secondToken.find_first_not_of("0123456789");
				if (secondTokenLastPos != string::npos)
					secondToken = secondToken.substr(0, secondTokenLastPos);
			}
			else
			{
				return; // Illegal line
			}

			if (!IOUtil::isInteger(firstToken) || !IOUtil::isInteger(secondToken))
			{
				return; // Illegal line
			}

			// Attempt to convert to numbers
			int firstNum = stoi(firstToken);
			int secondNum = stoi(secondToken);

			// Check if these are legal coordinates, otherwise ignore
			if ((firstNum >= 1) && (firstNum <= rows) &&
				(secondNum >= 1) && (secondNum <= cols))
			{
				auto move = std::make_pair(firstNum, secondNum);
				moveListPtr.push_back(move);
			}
		}
	};

	// Start parsing the file, line by line
	IOUtil::parseFile(filename, lineParser);
}

GameFromFileAlgo::GameFromFileAlgo(): IBattleshipGameAlgo()
{
}

void GameFromFileAlgo::setBoard(int player, const char** board, int numRows, int numCols)
{
	// Board itself is mostly ignored by the read from file logic
	// Save only the player number and board size
	_playerNum = player;
	_numRows = numRows;
	_numCols = numCols;
}

bool GameFromFileAlgo::init(const string& path)
{
	try
	{
		vector<string> attackFiles = IOUtil::listFilesInPath(path, ATTACK_SUFFIX);
		string attackFile = path + "\\";

		if (attackFiles.size() == 0)
		{
			return false;							   // No attack files found - init fails
		}
		else if (attackFiles.size() == 1)
		{
			attackFile += attackFiles.at(0);			   // 1 attack file found, use it
		}
		else
		{
			int fileIndex = (_playerNum == 0) ? 0 : 1; // Use first file for player0, otherwise second file
			attackFile += attackFiles.at(fileIndex);
		}

		// Build player move list from attack file contents
		populateMovesFromFile(attackFile);

		return true;
	}
	catch (...)
	{	// This should be a barrier that stops the app from failing.
		// If the algorithm fails to initialize due to an error, let it fail gracefully
		return false;
	}
}

std::pair<int, int> GameFromFileAlgo::attack()
{
	try
	{
		// Execute the next step from the attack file
		// If the player have used his entire move list read from the file, he forfeits
		// (allowing the other player to play until he wins or both forfeit)
		if (_predefinedMoves.empty())
			return battleship::NO_MORE_MOVES;

		std::pair<int, int> nextMove = *(_predefinedMoves.begin());	// Fetch next move from head of list
		_predefinedMoves.erase(_predefinedMoves.begin());	// Remove fetched item

		battleship::AttackValidator validator;
		return validator(nextMove, _numRows, _numCols);
	}
	catch (const exception& e)
	{	// This should be a barrier that stops the app from failing.
		// If the algorithm fails to attack due to an error, it should forfeit
		cerr << "Error: GameFromFileAlgo::attack failed on " << e.what() << endl;
		return NO_MORE_MOVES;
	}
}

void GameFromFileAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	// Ignored by the read from file logic
}

ALGO_API IBattleshipGameAlgo* GetAlgorithm()
{
	return new GameFromFileAlgo();
}