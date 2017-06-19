#include "AlgoLoader.h"
#include <functional>
#include <algorithm>
#include "IBattleshipGameAlgo.h"
#include "IOUtil.h"
#include "Logger.h"

using std::function;

namespace battleship
{
	void AlgoLoader::fetchDLLs(const string& path)
	{
		Logger::getInstance().log(Severity::DEBUG_LEVEL, "AlgoLoader Fetching list of available DLLs..");

		_availableGameAlgos = IOUtil::listFilesInPath(path, "dll");

		// Scan for dlls in the path
		for (auto& nextDllFilename : _availableGameAlgos)
		{
			Logger::getInstance().log(Severity::DEBUG_LEVEL, nextDllFilename + " found");
		}
	}

	void AlgoLoader::stripNameSuffix(string& name)
	{
		if (IOUtil::endsWith(name, DLL_SUFFIX_LONG))
		{
			IOUtil::removeSuffix(name, DLL_SUFFIX_LONG);
		}
		else if (IOUtil::endsWith(name, DLL_SUFFIX_SHORT))
		{
			IOUtil::removeSuffix(name, DLL_SUFFIX_SHORT);
		}
	}

	void AlgoLoader::loadAlgorithm(const string& algoName)
	{
		string algoFullpath = _algosPath + "\\" + algoName;

		// Load dynamic library
		// Again using Unicode compatible version of LoadLibrary
		HINSTANCE hDll = LoadLibraryA(algoFullpath.c_str());
		if (!hDll)
		{
			Logger::getInstance().log(Severity::WARNING_LEVEL, "Cannot load dll: " + algoFullpath);
			return;
		}

		// Get function pointer
		GetAlgorithmFuncType getAlgorithmFunc =
			reinterpret_cast<GetAlgorithmFuncType>(GetProcAddress(hDll, "GetAlgorithm"));
		if (!getAlgorithmFunc)
		{
			Logger::getInstance().log(Severity::WARNING_LEVEL, "Cannot load dll: " + algoFullpath);
			FreeLibrary(hDll); // Make sure to release loaded library, as AlgoLoader doesn't manage it yet
			return;
		}

		// Keep algorithm in list of loaded algos
		string algoFormattedName = algoName;
		stripNameSuffix(algoFormattedName);
		_loadedGameAlgos.emplace_back(algoFormattedName, hDll, getAlgorithmFunc); // Build algoDescriptor
		_loadedGameAlgoNames.push_back(algoFormattedName);

		Logger::getInstance().log(Severity::INFO_LEVEL, algoName + " loaded successfully");
	}

	AlgoLoader::AlgoLoader(const string& path): _algosPath(path)
	{	
		Logger::getInstance().log(Severity::INFO_LEVEL, "AlgoLoader started.. Loading from path: " + _algosPath);

		// Fetch DLL list
		fetchDLLs(path);
	}

	AlgoLoader::~AlgoLoader()
	{
		// Close all the dynamic libs we opened
		for (auto algIter = _loadedGameAlgos.begin(); algIter != _loadedGameAlgos.end(); ++algIter)
		{
			// Free HINSTANCE loaded, which resides in the 2nd cell of the algo tuple
			auto descriptor = *(algIter);
			Logger::getInstance().log(Severity::DEBUG_LEVEL, "Freeing algorithm: " + descriptor.path);
			FreeLibrary(descriptor.dll);
		}
	}

	const vector<string>& AlgoLoader::loadAllAvailableAlgorithms()
	{
		// Try to load all algorithms available
		for (const string& algoName : _availableGameAlgos)
		{
			loadAlgorithm(algoName);
		}

		return _loadedGameAlgoNames;
	}

	unique_ptr<IBattleshipGameAlgo> AlgoLoader::requestAlgo(const string& algoName) const
	{
		// Verify algo was already loaded before
		auto it = std::find_if(_loadedGameAlgos.begin(), _loadedGameAlgos.end(),
			[&algoName](AlgoDescriptor const& ad) { return ad.path == algoName; });

		if (it == _loadedGameAlgos.end())
		{
			// Not loaded before, meaning a wrong algoPath given
			Logger::getInstance().log(Severity::ERROR_LEVEL,
									  "Error: Trying to load algorithm from " + algoName +
									  " but this DLL isn't managed by the AlgoLoader");
			return nullptr;
		}
		
		// Algo's DLL loaded before
		// Retrieve algorithm descriptor & create an instance out of it
		auto algoDescriptor = *it;

		auto getAlgorithmFunc = algoDescriptor.algoFunc;

		// Call GetAlgorithm for the specified algorithm,
		// this should create a new instance for that algo type 
		IBattleshipGameAlgo* algo = getAlgorithmFunc();
		if (nullptr == algo)
		{
			Logger::getInstance().log(Severity::ERROR_LEVEL,
									  "Error: Cannot create instance out of dll of player: " + algoDescriptor.path);
			return nullptr;
		}

		Logger::getInstance().log(Severity::DEBUG_LEVEL, algoName + " new instance created");

		// Wrap in a smart pointer, so consumers don't have to deal with memory deallocation manually
		return unique_ptr<IBattleshipGameAlgo>(algo);
	}

	const vector<string>& AlgoLoader::availableGameAlgos() const
	{
		return _availableGameAlgos;
	}

	const vector<string>& AlgoLoader::loadedGameAlgos() const
	{
		return _loadedGameAlgoNames;
	}
}