#pragma once

#include <memory>
#include <string>

using std::shared_ptr;
using std::string;

namespace battleship
{
	class BattleBoard;

	/** Strategies for initializing the BattleBoard */
	enum class BattleshipBoardInitTypeEnum
	{
		LOAD_BOARD_FROM_FILE
	};

	/** 
	 * A factory class for instantiating BattleBoard classes using various methods
	 * (currently load from file, possibly in the future "set manually" or "random".
	 */
	class BattleshipGameBoardFactory
	{
	public:
		~BattleshipGameBoardFactory() = delete;	// Disallow allocation of this object
												// (more precisely - deallocation)
												// Any attempt to instantiate a factory by mistake
												// will result in a compilation error that protects

		/** Creates a BattleBoard using the loadMethod. This overload takes no parameters. */
		static shared_ptr<BattleBoard> loadBattleBoard(BattleshipBoardInitTypeEnum loadMethod);

		/** Creates a BattleBoard using the loadMethod. This overload takes 1 parameter (e.g: filename) */
		static shared_ptr<BattleBoard> loadBattleBoard(BattleshipBoardInitTypeEnum loadMethod, const string& param0);

	private:

		BattleshipGameBoardFactory() = default;	// This class shouldn't be instantiated

		/** Builds a BattleBoard by parsing the input board file using a BoardBuilder helper object. */
		static shared_ptr<BattleBoard> buildBoardFromFile(const string& path);
	};
}