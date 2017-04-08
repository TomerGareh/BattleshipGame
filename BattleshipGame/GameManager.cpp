#include "GameManager.h"

namespace battleship
{
	GameManager::GameManager()
	{
	}


	GameManager::~GameManager()
	{
	}

	bool GameManager::isPlayerShipsLeft(PlayerEnum player)
	{

	}

	bool GameManager::isGameOver()
	{

	}

	void GameManager::startGame(shared_ptr<BattleBoard> board,
								IBattleshipGameAlgo& playerA, IBattleshipGameAlgo& playerB,
								IGameVisual& visualizer)
	{
		playerA.setBoard(board->getBoardMatrix(), BOARD_SIZE, BOARD_SIZE);
		playerB.setBoard(board->getBoardMatrix(), BOARD_SIZE, BOARD_SIZE);
	}
}