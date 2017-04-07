#include "BoardBuilder.h"

namespace battleship
{
	BoardBuilder::BoardBuilder()
	{
		_board = std::make_shared<BattleBoard>();	// Only BoardBuilder can instantiate this class
	}


	BoardBuilder::~BoardBuilder()
	{
	}

	BoardBuilder* BoardBuilder::addPiece(int x, int y, char type)
	{
		_board->initSquare(x, y, type);
		return this;
	}

	void BoardBuilder::printErrors()
	{
		// TODO: Check if there are errors and if so print them
	}

	bool BoardBuilder::validate()
	{
		ErrorPriorityFunction sortFunc = [](const BoardInitializeError& err1, const BoardInitializeError& err2)
		{
			return err1.getPriority() < err2.getPriority();
		};

		// Initialize a "sorted set" which functions as a queue that automatically sorts errors by priority.
		// This is a set so errors can only be repeated once.
		set<BoardInitializeError, ErrorPriorityFunction> errorQueue(sortFunc);
		
		// TODO: Call validation process here, add errors to errorQueue

		// TODO: when a game piece is verified, add it to the board using this api
		// void _board->addGamePiece(int firstX, int firstY, int size,
		//								  PlayerEnum player, Orientation orientation)

		printErrors();

		return errorQueue.empty();	// Empty error queue means the board is valid
	}

	shared_ptr<BattleBoard> BoardBuilder::build()
	{
		bool isBoardValid = validate();
		return isBoardValid ? _board : NULL;
	}
}