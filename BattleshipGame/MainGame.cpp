#include "MainGame.h"
#include "MainBattleshipGame.h"
#include <iostream>
#include <exception>

using std::cerr;
using std::endl;
using std::exception;

int main(int argc, char* argv[])
{
	try
	{
		battleship::MainBattleshipGame::run(argc, argv);
	}
	catch (const exception& e)
	{	// This should be the last barrier that stops the app from failing,
		// We don't log here because anything can cause this error - including the logger itself.
		cerr << "Error: General error of type " << e.what() << endl;
		return battleship::MainBattleshipGame::ERROR_CODE;
	}
}