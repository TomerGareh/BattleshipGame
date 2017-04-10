#pragma once

#include <functional>
#include <string>

namespace battleship
{
	using std::string;
	using std::function;

	/** A delimeter character to be used for attack files, to separate pair of coordinates */
	const char COORDS_DELIM = ',';

	/** A helper class for dealing with IO operations logic common to the entire battleship game. */
	class IOUtil
	{
	public:
		virtual ~IOUtil() = delete;	// Disallow allocation of this helper object
									// (more precisely - deallocation)

		/** Replaces all illegal characters - which are all characters that are not digits or COORDS_DELIM
		 *  with the replacementChar given in the line string.
		 */
		static void replaceIllegalCharacters(string& line, const char replacementChar);

		/** Removes all leading and trailing space and tab characters from the string.
		 */
		static void removeLeadingTrailingSpaces(string& line);

		/** Parses the file line by line and invokes the lineParser on each line read.
		 *  This helper function deals with IO errors and managing external file resources.
		 */
		static void parseFile(const string& filename, function<void(string& nextReadLine)> lineParser);

		static void loadFilesInPath(const string& path);

	private:
		IOUtil() = default;	// This helper class shouldn't be instantiated
	};
}