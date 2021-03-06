#pragma once

#include <functional>
#include <initializer_list>
#include <vector>

namespace battleship
{
	using std::string;
	using std::function;
	using std::initializer_list;
	using std::vector;

	/** A helper class for dealing with IO operations logic common to the entire battleship game. */
	class IOUtil
	{
	public:
		virtual ~IOUtil() = delete;	// Disallow allocation of this helper object
									// (more precisely - deallocation)

		/** Returns if the given string can be safely converted to an integer or not */
		static bool isInteger(const std::string & s);

		/** Returns true if the line contains only whitespaces: space, tab characters, etc */
		static bool isContainOnlyWhitespaces(const string& str);

		/** Replaces all illegal characters - which are all characters that are not specified in legalChars
		 *  with the replacementChar given in the line string.
		 */
		static void replaceIllegalCharacters(string& line, const char replacementChar, initializer_list<char> legalChars);

		/** Removes all leading and trailing space and tab characters from the string.
		 */
		static void removeLeadingTrailingSpaces(string& line);

		/** Parses the file line by line and invokes the lineParser on each line read.
		 *  If a headerParser is specified the header can be parsed by a different logic,
		 *	which sets isHeader=false when we're done finishing reading the header, or 
		 *  isValidFile=false if the file header is invalid.
		 *  This helper function deals with IO errors and managing external file resources.
		 *  Returns true for a valid file, false for an invalid file or on io errors.
		 */
		static bool parseFile(const string& filename,
					  	      function<void(string& nextReadLine)> lineParser,
							  function<void(string& nextReadLine, int lineNum,
										    bool& isHeader, bool& isValidFile)> headerParser = NULL);

		/** Returns true if fullString starts with prefix */
		static bool startsWith(const string& fullString, const string& prefix);

		/** Returns true if fullString ends with ending as a suffix */
		static bool endsWith(const string& fullString, const string& ending);

		/** Removes the prefix from the fullString. Assumes the fullString begins with prefix,
		 *  otherwise behaviour is undefined.
		 */
		static void removePrefix(string& fullString, const string& prefix);

		/** Removes the suffix from the fullString. Assumes the fullString ends with suffix,
		 *  otherwise behaviour is undefined.
		 */
		static void removeSuffix(string& fullString, const string& suffix);

		/** Returns true if the path argument points to a real valid path on disk, false if not. */
		static bool validatePath(const string& path);

		/** Lists all files found in the given path with the given extension (without a dot, e.g. "sboard").
		 *  Path is expected to be valid (e.g verified with "validatePath").
		 *  If no files with the given extension are found, an empty vector is returned.
		 *  The files returned will be sorted in lexicographical order.
		 */
		static vector<string> IOUtil::listFilesInPath(const string& path, const string& extension);

		/** Converts the given path to "absolute" path on the disk.
		 *  If the given path is already absolute, this function simply returns it.
		 *  This function does not verify that the resulting path is valid.
		 */
		static string convertPathToAbsolute(const string& path);

	private:
		IOUtil() = default;	// This helper class shouldn't be instantiated
	};
}