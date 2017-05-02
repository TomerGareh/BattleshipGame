#include "GameFromFileAlgo.h"
#include "IOUtil.h"
#include "BattleBoard.h"
#include "AlgoCommon.h"

using namespace battleship;

const string GameFromFileAlgo::ATTACK_SUFFIX = ".attack";

// TODO: Change to coordinates of 1 to 10!
void GameFromFileAlgo::populateMovesFromFile(const string& filename)
{
	// This line-parser logic will clean the line string from illegal characters and spaces,
	// and then attempt to break it to tokens and look for a pair of numbers.
	// If a pair of legal coordinates is encountered, we add it to the move list.
	// We pass a local ref to the object's field: _predefinedMoves, because this single field can't
	// be captured inside the lambda expression (we have to capture "this" if we want to allow access,
	// instead we capture a "fake" reference to the _predefinedMoves field)
	auto* moveListPtr = &_predefinedMoves;
	auto lineParser = [moveListPtr](string& nextLine) mutable
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
			if ((firstNum >= 1) && (firstNum <= BOARD_SIZE) &&
				(secondNum >= 1) && (secondNum <= BOARD_SIZE))
			{
				auto move = std::make_pair(firstNum, secondNum);
				moveListPtr->push_back(move);
			}
		}
	};

	// Start parsing the file, line by line
	IOUtil::parseFile(filename, lineParser);
}

GameFromFileAlgo::GameFromFileAlgo(const string& attackFile): IBattleshipGameAlgo()
{
}

void GameFromFileAlgo::setBoard(int player, const char** board, int numRows, int numCols)
{
	// Mostly ignored by the read from file logic
	// Save only the player number
	_playerNum = player;
}

bool GameFromFileAlgo::init(const string& path)
{
	vector<string> attackFiles = IOUtil::listFilesInPath(path, ATTACK_SUFFIX);
	string attackFile;

	if (attackFiles.size() == 0)
	{
		return false;							   // No attack files found - init fails
	}
	else if (attackFiles.size() == 1)
	{
		attackFile = attackFiles.at(0);			   // 1 attack file found, use it
	}
	else
	{
		int fileIndex = (_playerNum == 0) ? 0 : 1; // Use first file for player0, otherwise second file
		attackFile = attackFiles.at(fileIndex);
	}

	// Build player move list from attack file contents
	populateMovesFromFile(attackFile);

	return true;
}

std::pair<int, int> GameFromFileAlgo::attack()
{
	// Execute the next step from the attack file
	// If the player have used his entire move list read from the file, he forfeits
	// (allowing the other player to play until he wins or both forfeit)
	if (_predefinedMoves.empty())
		return battleship::NO_MORE_MOVES;

	std::pair<int, int> nextMove = *(_predefinedMoves.begin());	// Fetch next move from head of list
	_predefinedMoves.erase(_predefinedMoves.begin());	// Remove fetched item

	return nextMove;
}

void GameFromFileAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	// Ignored by the read from file logic
}

ALGO_API IBattleshipGameAlgo* GetAlgorithm()
{
	// TODO: Load attack file here
	string attackFile = "";
	return new GameFromFileAlgo(attackFile);
}