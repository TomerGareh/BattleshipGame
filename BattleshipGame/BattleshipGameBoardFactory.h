#pragma once

#include <memory>
#include <string>

using std::shared_ptr;
using std::string;

namespace battleship
{
	class BattleBoard;

	/** Strategies for initializing the BattleBoard.
	 *	In the future more strategies can be added here, e.g: manually set board, randomize board, etc.
	 */
	enum class BattleshipBoardInitTypeEnum
	{
		LOAD_BOARD_FROM_FILE
	};

	/** 
	 * A factory class for instantiating BattleBoard classes using various methods
	 * (currently load from file, possibly in the future "set manually" or "random").
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

		/** Creates a BattleBoard using the loadMethod. This overload takes 1 parameter (e.g: filename),
		 *  according to the strategy specified.
		 */
		static shared_ptr<BattleBoard> loadBattleBoard(BattleshipBoardInitTypeEnum loadMethod, const string& param0);

	private:
		/** Suffix for game board files **/
		static const string BOARD_SUFFIX;

		BattleshipGameBoardFactory() = default;	// This class shouldn't be instantiated

		/** Builds a BattleBoard by parsing the input board file path using a BoardBuilder helper object.
		 *  path is an argument that specifies where board files are expected to exist on the disk.
		 *	If the path is invalid or no board files are found, errors are printed and NULL is returned.
		 */
		static shared_ptr<BattleBoard> buildBoardFromFile(const string& path);
	};
}