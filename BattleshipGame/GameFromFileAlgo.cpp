#include "GameFromFileAlgo.h"
#include "IOUtil.h"
#include "BattleBoard.h"

using namespace battleship;
using std::string;

void GameFromFileAlgo::populateMovesFromFile(string& filename)
{
	// This line-parser logic will clean the line string from illegal characters and spaces,
	// and then attempt to break it to tokens and look for a pair of numbers.
	// If a pair of legal coordinates is encountered, we add it to the move list.
	// We pass a local ref to the object's field: _predefinedMoves, because this single field can't
	// be captured inside the lambda expression (we have to capture "this" if we want to allow access,
	// instead we capture a "fake" reference to the _predefinedMoves field)
	auto& moveListRef = _predefinedMoves;
	auto lineParser = [moveListRef](string& nextLine) mutable
	{
		IOUtil::replaceIllegalCharacters(nextLine, ' ');
		IOUtil::removeLeadingTrailingSpaces(nextLine);

		// Search for punctuation delimeter, if not found this is an illegal row, skip it (do nothing)
		size_t delim_pos;
		if ((delim_pos = nextLine.find(COORDS_DELIM)) != string::npos)
		{
			// Break to tokens
			string firstToken = nextLine.substr(0, delim_pos);
			nextLine.erase(0, delim_pos + 1);
			string& secondToken = nextLine;

			// Attempt to convert to numbers
			int firstNum = stoi(firstToken);
			int secondNum = stoi(secondToken);

			// Check if these are legal coordinates
			if ((firstNum >= 1) && (firstNum <= BOARD_SIZE) &&
				(secondNum >= 1) && (secondNum <= BOARD_SIZE))
			{
				auto move = std::make_pair(firstNum, secondNum);
				moveListRef.push_back(move);
			}
		}
	};

	// Start parsing the file, line by line
	IOUtil::parseFile(filename, lineParser);
}

GameFromFileAlgo::GameFromFileAlgo(string& attackFile): IBattleshipGameAlgo()
{
	// Build player move list from attack file contents
	populateMovesFromFile(attackFile);
}

void GameFromFileAlgo::setBoard(const char** board, int numRows, int numCols)
{
	// Ignored by the read from file logic
}

std::pair<int, int> GameFromFileAlgo::attack()
{
	// Execute the next step from the attack file
	// If the player have used his entire move list read from the file, he forfeits
	// (allowing the other player to play until he wins or both forfeit)
	if (_predefinedMoves.empty())
		return FORFEIT;

	std::pair<int, int> nextMove = *(_predefinedMoves.begin());	// Fetch next move from head of list
	_predefinedMoves.erase(_predefinedMoves.begin());	// Remove fetched item

	return nextMove;
}

void GameFromFileAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	// Ignored by the read from file logic
}