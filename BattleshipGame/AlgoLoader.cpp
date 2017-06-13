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

		_availableGameAlgos.clear();
		vector<string> foundDlls = IOUtil::listFilesInPath(path, "dll");

		// Scan for dlls in the path, and append the path to their filenames
		for (auto& nextDllFilename : foundDlls)
		{
			string fullFileName = path + "\\" + nextDllFilename;
			_availableGameAlgos.push_back(fullFileName);
			Logger::getInstance().log(Severity::DEBUG_LEVEL, fullFileName + " found");
		}
	}

	void AlgoLoader::loadAlgorithm(const string& algoFullpath)
	{
		// Load dynamic library
		// Again using Unicode compatible version of LoadLibrary
		HINSTANCE hDll = LoadLibraryA(algoFullpath.c_str());
		if (!hDll)
		{
			Logger::getInstance().log(Severity::WARNING_LEVEL, "Cannot load dll: " + algoFullpath);
			return;
		}

		// Get function pointer
		GetAlgorithmFuncType getAlgorithmFunc = (GetAlgorithmFuncType)GetProcAddress(hDll, "GetAlgorithm");
		if (!getAlgorithmFunc)
		{
			Logger::getInstance().log(Severity::WARNING_LEVEL, "Cannot load dll: " + algoFullpath);
			FreeLibrary(hDll); // Make sure to release loaded library, as AlgoLoader doesn't manage it yet
			return;
		}

		// Keep algorithm in list of loaded algos
		auto algoDescriptor = std::make_shared<AlgoDescriptor>(algoFullpath, hDll, getAlgorithmFunc);
		_loadedGameAlgos.push_back(algoDescriptor);
		_loadedGameAlgoNames.push_back(algoFullpath);

		Logger::getInstance().log(Severity::INFO_LEVEL, algoFullpath + " loaded successfully");
	}

	AlgoLoader::AlgoLoader(const string& path)
	{	
		Logger::getInstance().log(Severity::INFO_LEVEL, "AlgoLoader started..");

		// Fetch DLL list
		fetchDLLs(path);

		// Try to load all algorithms available
		for (const string& algoPath : _availableGameAlgos)
		{
			loadAlgorithm(algoPath);
		}
	}

	AlgoLoader::~AlgoLoader()
	{
		// Close all the dynamic libs we opened
		for (auto algIter = _loadedGameAlgos.begin(); algIter != _loadedGameAlgos.end(); ++algIter)
		{
			// Free HINSTANCE loaded, which resides in the 2nd cell of the algo tuple
			auto descriptor = *(algIter);
			Logger::getInstance().log(Severity::DEBUG_LEVEL, "Freeing algorithm: " + descriptor->path);
			FreeLibrary(descriptor->dll);
		}

		Logger::getInstance().log(Severity::INFO_LEVEL, "AlgoLoader destroyed..");
	}

	shared_ptr<IBattleshipGameAlgo> AlgoLoader::requestAlgo(const string& algoPath) const
	{
		shared_ptr<AlgoDescriptor> algoDescriptor = NULL;

		// Verify algo was already loaded before
		auto it = std::find_if(_loadedGameAlgos.begin(), _loadedGameAlgos.end(),
			[&algoPath](shared_ptr<AlgoDescriptor> const& ad) { return ad->path == algoPath; });

		if (it == _loadedGameAlgos.end())
		{
			// Not loaded before, meaning a wrong algoPath given
			Logger::getInstance().log(Severity::ERROR_LEVEL,
									  "Error: Trying to load algorithm from " + algoPath +
									  " but this DLL isn't managed by the AlgoLoader");
			return NULL;
		}
		else
		{	// Algo's DLL loaded before
			// Retrieve algorithm descriptor & create an instance out of it
			algoDescriptor = *it;
		}

		auto getAlgorithmFunc = algoDescriptor->algoFunc;

		// Call GetAlgorithm for the specified algorithm,
		// this should create a new instance for that algo type 
		IBattleshipGameAlgo* algo = getAlgorithmFunc();
		if (NULL == algo)
		{
			Logger::getInstance().log(Severity::ERROR_LEVEL,
									  "Error: Cannot create instance out of dll: " + algoDescriptor->path);
			return NULL;
		}

		Logger::getInstance().log(Severity::DEBUG_LEVEL, algoPath + " new instance created");

		// Wrap in a smart pointer, so consumers don't have to deal with memory deallocation manually
		return shared_ptr<IBattleshipGameAlgo>(algo);
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