#pragma once

#include <memory>

using std::unique_ptr;
using std::shared_ptr;

namespace battleship
{
	class IBattleshipGameAlgo;
	class BattleBoard;

	enum class BattleshipGameAlgoTypeEnum
	{
		FILE_GAME_ALGO
	};

	enum class BattleshipBoardInitTypeEnum
	{
		LOAD_BOARD_FROM_FILE
	};

	class BattleshipGameAlgoFactory
	{
	public:
		BattleshipGameAlgoFactory() = delete;
		~BattleshipGameAlgoFactory() = default;

		static unique_ptr<IBattleshipGameAlgo> createGameAlgo(BattleshipGameAlgoTypeEnum algoType);

		static shared_ptr<BattleBoard> loadBattleBoard(BattleshipBoardInitTypeEnum loadMethod);
		static shared_ptr<BattleBoard> loadBattleBoard(BattleshipBoardInitTypeEnum loadMethod, const string& param0);

	private:

		static shared_ptr<BattleBoard> buildBoardFromFile(const string& path);
	};
}