#include "Configuration.h"
#include "IOUtil.h"
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

namespace battleship
{
	/** Fills the configuration dictionary with arguments that arrive from the command line
	 */
	bool Configuration::parseArgs(int argc, char* argv[])
	{
		if (argc > MAX_ARG_COUNT)
		{
			cerr << "Error: Too many arguments given. Try: BattleShipGame [path] [-threads <#count>]" << endl;
			return false;
		}

		// If the first parameter doesn't match any of the configuration keywords this is a path
		if ((argc >= 2) && strcmp(BP_CONFIG_THREADS, argv[1]))
		{
			this->path = argv[1];
		}

		// Digest rest of program arguments
		for (int i = 1; i < argc; i++)
		{
			if (!strcmp(argv[i], BP_CONFIG_THREADS))
			{
				if (argc > i + 1)
				{
					string argVal = argv[i + 1];
					if (IOUtil::isInteger(argVal))
					{
						this->threads = std::stoi(argv[i + 1]);

						if (this->threads <= 0)	// Invalid thread count value
						{
							cerr << "Error: Invalid thread count value. Thread count must be > 0 " << endl;
							return false;
						}
					}
					else
					{
						cerr << "Error: Illegal threads field value. Try: -threads <#count>" << endl;
						return false;
					}
				}
				else
				{
					cerr << "Error: Threads argument missing value field. Try: -threads <#count>" << endl;
					return false;
				}
			}
		}

		return true;
	}

	void Configuration::normalizeValue(string& value)
	{
		// Nothing to normalize
		if (value.empty())
			return;

		// IF starts and ends with value marker ("quotation marks") - remove them
		if (IOUtil::startsWith(value, CONFIG_VALUE_MARKER) &&
			IOUtil::endsWith(value, CONFIG_VALUE_MARKER))
		{
			value.erase(0, 1);
			value.erase(value.length() - 1, 1);
		}
	}

	bool Configuration::loadConfigFile()
	{
		auto config = [this](string& nextLine, int lineNum, bool& isHeader, bool& isValidFile)
		{
			isValidFile = true;
			IOUtil::removeLeadingTrailingSpaces(nextLine);

			if (IOUtil::startsWith(nextLine, CONFIG_HEADER_PATH)) // Path parameter (string)
			{
				IOUtil::removePrefix(nextLine, CONFIG_HEADER_PATH);
				normalizeValue(nextLine);
				this->path = nextLine;
			}
			else if (IOUtil::startsWith(nextLine, CONFIG_HEADER_THREADS)) // Threads parameter (int)
			{
				IOUtil::removePrefix(nextLine, CONFIG_HEADER_THREADS);
				normalizeValue(nextLine);

				if (IOUtil::isInteger(nextLine)) // Only use the value if this is a valid int
				{
					this->threads = std::stoi(nextLine.c_str());

					if (this->threads <= 0)	// Invalid thread count value
						isValidFile = false;
				}
				else
				{
					isValidFile = false;
				}
			}
			else if ((IOUtil::startsWith(nextLine, CONFIG_HEADER_COMMENT)) ||  // Comment %%
					 (IOUtil::isContainOnlyWhitespaces(nextLine))) // Empty line
			{
				// Ignore this line
			}
			else // Unknown parameter
			{
				isValidFile = false;
			}

			isHeader = true; // The entire configuration file is treated as a header to ensure validity
		};

		// Configuration file has no body
		auto emptyBodyParser = [this](string& nextLine) {};

		bool isValidConfigFile = IOUtil::parseFile(CONFIG_FILE, emptyBodyParser, config);

		return isValidConfigFile;
	}

	void Configuration::loadDefaults()
	{
		this->path = DEFAULT_PATH;			   // Nameless param, default is working directory
		this->threads = DEFAULT_THREAD_COUNT;  // Optional param: worker threads count
	}

	Configuration::Configuration()
	{
		// First try to load configuration from command line args
		if (!loadConfigFile())
		{
			// If it fails, load defaults
			loadDefaults();
		}
	}
}