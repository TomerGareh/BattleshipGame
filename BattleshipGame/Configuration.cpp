#include "Configuration.h"
#include "IOUtil.h"
#include <iostream>
#include <climits>

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
			string error = "Error: Too many arguments given. Try: BattleShipGame [path] [-threads <#count>]";
			configurationIssues.push_back(std::make_pair(Severity::ERROR_LEVEL, error));
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
							string error = "Error: Invalid thread count value. Thread count must be > 0 ";
							configurationIssues.push_back(std::make_pair(Severity::ERROR_LEVEL, error));
							return false;
						}
					}
					else
					{
						string error = "Error: Illegal threads field value. Try: -threads <#count>";
						configurationIssues.push_back(std::make_pair(Severity::ERROR_LEVEL, error));
						return false;
					}
				}
				else
				{
					string error = "Error: Threads argument missing value field. Try: -threads <#count>";
					configurationIssues.push_back(std::make_pair(Severity::ERROR_LEVEL, error));
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

		// If starts and ends with value marker ("quotation marks") - remove them
		if (IOUtil::startsWith(value, CONFIG_VALUE_MARKER) &&
			IOUtil::endsWith(value, CONFIG_VALUE_MARKER))
		{
			value.erase(0, 1);
			value.erase(value.length() - 1, 1);
		}
	}

	bool validateInt(const string& parsedArg, int min, int max)
	{
		if (!IOUtil::isInteger(parsedArg)) // Only use the value if this is a valid int
		{
			return false;
		}
		else
		{
			int val = std::stoi(parsedArg.c_str());

			if ((val < min) || (val > max))	// Invalid value
			{
				return false;
			}
		}

		return true;
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

				if (validateInt(nextLine, 1, INT_MAX)) // Only use the value if this is a valid int
				{
					this->threads = std::stoi(nextLine.c_str());
				}
				else
				{
					isValidFile = false;
					string warning = "Configuration file traced invalid worker threads count value";
					configurationIssues.push_back(std::make_pair(Severity::WARNING_LEVEL, warning));
				}
			}
			else if (IOUtil::startsWith(nextLine, CONFIG_HEADER_LOGLEVEL)) // Log level parameter (int)
			{
				IOUtil::removePrefix(nextLine, CONFIG_HEADER_LOGLEVEL);
				normalizeValue(nextLine);

				int minVal = static_cast<int>(Severity::DEBUG_LEVEL);
				int maxVal = static_cast<int>(Severity::ERROR_LEVEL);

				if (validateInt(nextLine, minVal, maxVal)) // Only use the value if this is a valid int
				{
					this->logSeverity = static_cast<Severity>(std::stoi(nextLine.c_str()));
				}
				else
				{
					isValidFile = false;
					string warning = "Configuration file traced invalid log level value";
					configurationIssues.push_back(std::make_pair(Severity::WARNING_LEVEL, warning));
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
				string warning = "Configuration file traced an invalid configuration attribute";
				configurationIssues.push_back(std::make_pair(Severity::WARNING_LEVEL, warning));
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
		this->logSeverity = DEFAULT_SEVERITY;  // Default is info level
	}

	Configuration::Configuration()
	{
		// First load defaults in case configuration file is missing anything
		loadDefaults();

		// Next try to load configuration from file
		if (!loadConfigFile())
		{
			string warning = "Warning: Configuration file is missing or invalid. Loading default values..";
			configurationIssues.push_back(std::make_pair(Severity::WARNING_LEVEL, warning));

			// If it fails, load defaults again
			loadDefaults();
		}

		// Later on we may override with arguments from command line
	}
}