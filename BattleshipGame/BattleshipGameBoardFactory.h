#pragma once

#include <memory>
#include <string>

using std::shared_ptr;
using std::string;

namespace battleship
{
	class BattleBoard;

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
		static vector<shared_ptr<BattleBoard>> loadAllBattleBoards(const string& path);

		/** Creates a BattleBoard using the loadMethod. This overload takes 1 parameter (e.g: path),
		 *  according to the strategy specified. Returns the first battleboard encountered in lex order.
		 */
		static shared_ptr<BattleBoard> loadBattleBoard(const string& path);

	private:
		/** Suffix for game board files **/
		static const string BOARD_SUFFIX;

		BattleshipGameBoardFactory() = default;	// This class shouldn't be instantiated

		/** Parse header of battleboard file.
		 *  nextLine contains the header line, rows, cols, depth will contain the resulting dimensions parsed.
		 */
		static bool parseHeader(string& nextLine, int& rows, int& cols, int& depth);

		/** Parses a line of data from the board file.
		 *	The line is expected to contain actual data with game pieces.
		 *  builder is the BoardBuilder used to convert the board file to a real board object,
		 *  nextLine contains the line string, depthIndex and rowIndex are the current indices of row and depth
		 *  within the board and "cols" is the total width of the board as parsed from the header.
		 */
		static void BattleshipGameBoardFactory::parseBoardRow(BoardBuilder& builder, string& nextLine,
															  int depthIndex, int rowIndex, int cols);

		/** Builds a BattleBoard by parsing the input board file path using a BoardBuilder helper object.
		 *  path is an argument that specifies where board files are expected to exist on the disk.
		 *	If the path is invalid or no board files are found, errors are printed and NULL is returned.
		 */
		static shared_ptr<BattleBoard> buildBoardFromFile(const string& path);
	};
}