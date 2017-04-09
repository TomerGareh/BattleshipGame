#pragma once

#include <memory>
#include <string>

using std::shared_ptr;
using std::string;

namespace battleship
{
	class BattleBoard;

	enum class BattleshipBoardInitTypeEnum
	{
		LOAD_BOARD_FROM_FILE
	};

	class BattleshipGameBoardFactory
	{
	public:
		~BattleshipGameBoardFactory() = delete;	// Disallow allocation of this object
												// (more precisely - deallocation)

		static shared_ptr<BattleBoard> loadBattleBoard(BattleshipBoardInitTypeEnum loadMethod);
		static shared_ptr<BattleBoard> loadBattleBoard(BattleshipBoardInitTypeEnum loadMethod, const string& param0);

	private:

		BattleshipGameBoardFactory() = default;	// This class shouldn't be instantiated
		static shared_ptr<BattleBoard> buildBoardFromFile(const string& path);
	};
}