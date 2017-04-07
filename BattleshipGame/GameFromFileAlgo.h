#pragma once

#include <memory>
#include <list>
#include <tuple>
#include "IBattleshipGameAlgo.h"
#include "BattleBoard.h"


typedef std::list<std::tuple<int, int, char>> ShipMaskList;
typedef std::unique_ptr<ShipMaskList> ShipMaskListPtr;

enum class ShipType {
	RubberBoat, RocketShip, Submarine, Battleship
};

class GameFromFileAlgo : IBattleshipGameAlgo
{
private:
	class ShipMask
	{
		ShipMaskListPtr mask;
		
		ShipMask(ShipType ship);

		~ShipMask();

		bool applyMask(const char board[BOARD_SIZE][BOARD_SIZE], std::tuple<int, int> pos, bool isPlayerA,
					   bool& wrongSize, bool& adjacentShips, bool& horizontalMatch);

		friend class GameFromFileAlgo;
	};

	bool isValidBoard(const char board[BOARD_SIZE][BOARD_SIZE]);

public:
	GameFromFileAlgo();
	
	~GameFromFileAlgo();
	
	void setBoard(const char** board, int numRows, int numCols) override;
	
	std::pair<int, int> attack() override;
	
	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

	void readBoardFile(const std::string& fileName, char board[BOARD_SIZE][BOARD_SIZE]);
};