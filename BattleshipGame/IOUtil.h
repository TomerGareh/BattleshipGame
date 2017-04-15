#pragma once

#include <memory>
#include <string>
#include <functional>
#include <initializer_list>
#include <map>

namespace battleship
{
	using std::shared_ptr;
	using std::string;
	using std::function;
	using std::initializer_list;
	using std::map;

	/** A helper class for dealing with IO operations logic common to the entire battleship game. */
	class IOUtil
	{
	public:
		virtual ~IOUtil() = delete;	// Disallow allocation of this helper object
									// (more precisely - deallocation)

		/** Replaces all illegal characters - which are all characters that are not specified in legalChars
		 *  with the replacementChar given in the line string.
		 */
		static void replaceIllegalCharacters(string& line, const char replacementChar, initializer_list<char> legalChars);

		/** Removes all leading and trailing space and tab characters from the string.
		 */
		static void removeLeadingTrailingSpaces(string& line);

		/** Parses the file line by line and invokes the lineParser on each line read.
		 *  This helper function deals with IO errors and managing external file resources.
		 */
		static void parseFile(const string& filename, function<void(string& nextReadLine)> lineParser);

		/** Returns true if fullString ends with ending as a suffix */
		static bool endsWith(const string& fullString, const string& ending);

		/** Loads all files needed by the battleship game.
		 *  Returns a mapping of each of the file suffixes vs their path.
		 *	If one of the files is missing, NULL is returned.
		 */
		static shared_ptr<map<string, string>> loadFilesInPath(const string& path);

		// Suffixes for game files
		static const string BOARD_SUFFIX;
		static const string ATTACK_A_SUFFIX;
		static const string ATTACK_B_SUFFIX;

	private:
		IOUtil() = default;	// This helper class shouldn't be instantiated

		/** Prints missing files to the console */
		static void printLoadFileErrors(bool missingBoardFile, bool missingAttackAFile, bool missingAttackBFile,
										const string& pathToDisplay);
	};
}