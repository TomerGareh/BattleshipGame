#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include "BattleBoard.h"

using std::shared_ptr;
using std::unordered_map;
using std::string;
using std::vector;

namespace battleship
{
	/** 
	 * A factory class for instantiating BattleBoard classes using various methods
	 * (currently load from file, possibly in the future "set manually" or "random").
	 */
	class BattleshipGameBoardFactory
	{
	public:
		/** Creates a factory that initializes itself with a list of boards from the given path. 
		 *  Boards will be created and validated immediately when the factory is created.
		 */
		BattleshipGameBoardFactory(const string& path);
		~BattleshipGameBoardFactory() = default;

		/** Loads and validates all available battleboard files */
		const vector<string>& loadAllBattleBoards();

		/** Creates a BattleBoard instance using prototype pattern.
		 *  This method assumes "path" refers a valid battleboard that was loaded before,
		 *	as this function simply returns a new instance clone out of the template object.
		 *  For invalid board paths, NULL is returned.
		 */
		shared_ptr<BattleBoard> requestBattleboard(const string& path);

		/** Returns list of boards available for loading (not necessarily valid) */
		const vector<string>& availableBoardsList() const;

		/** Returns list of boards available for creation */
		const vector<string>& loadedBoardsList() const;

	private:
		/** Suffix for game board files **/
		static const string BOARD_SUFFIX;

		using LoadedBoardsIndex = unordered_map<string, unique_ptr<BattleBoard>>;

		/** Index of loaded board templates, for creating additional instances from prototypes */
		LoadedBoardsIndex _loadedBoards;

		/** List of available board files for loading (not necessarily valid) */
		vector<string> _availableBoards;

		/** Lists of boards available for creation */
		vector<string> _loadedBoardNames;

		/** Path to load board files from */
		string _path;

		/** Parse header of battleboard file.
		 *  nextLine contains the header line, rows, cols, depth will contain the resulting dimensions parsed.
		 */
		bool parseHeader(string& nextLine, int& rows, int& cols, int& depth) const;

		/** Parses a line of data from the board file.
		 *	The line is expected to contain actual data with game pieces.
		 *  builder is the BoardBuilder used to convert the board file to a real board object,
		 *  nextLine contains the line string, depthIndex and rowIndex are the current indices of row and depth
		 *  within the board and "cols" is the total width of the board as parsed from the header.
		 */
		static void parseBoardRow(BoardBuilder& builder, string& nextLine,
						   int depthIndex, int rowIndex, int cols);

		/** Builds a BattleBoard by parsing the input board file path using a BoardBuilder helper object.
		 *  path is an argument that specifies where board files are expected to exist on the disk.
		 *	If the path is invalid or no board files are found, errors are printed and NULL is returned.
		 */
		unique_ptr<BattleBoard> buildBoardFromFile(const string& path);
	};
}