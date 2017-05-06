#include "AlgoLoader.h"
#include <iostream>
#include <functional>
#include <algorithm>
#include "IBattleshipGameAlgo.h"
#include "IOUtil.h"

using std::cout;
using std::endl;
using std::function;

namespace battleship
{
	const string AlgoLoader::LOAD_DLL_ERROR_STRING = "Cannot load dll: ";
	const string AlgoLoader::MISSING_DLL_ERROR_STRING = "Missing an algorithm (dll) file looking in path: ";
	const string AlgoLoader::NON_EXISTING_ALGO_ERROR_STRING = "Error: Game tried to access a non-existing algorithm";

	AlgoLoader::AlgoLoader()
	{	
	}

	AlgoLoader::~AlgoLoader()
	{
		// Close all the dynamic libs we opened
		for (auto algIter = _loadedGameAlgos.begin(); algIter != _loadedGameAlgos.end(); ++algIter)
		{
			// Free HINSTANCE loaded, which resides in the 2nd cell of the algo tuple
			FreeLibrary(std::get<1>(*algIter));
		}

		_loadedGameAlgos.clear();
	}

	IBattleshipGameAlgo* AlgoLoader::loadAlgorithm(const string& algoFullpath)
	{
		// Load dynamic library
		// Again using Unicode compatible version of LoadLibrary
		HINSTANCE hDll = LoadLibraryA(algoFullpath.c_str());
		if (!hDll)
		{
			cout << LOAD_DLL_ERROR_STRING << algoFullpath << endl;
			return NULL;
		}

		// Get function pointer
		GetAlgorithmFuncType getAlgorithmFunc = (GetAlgorithmFuncType)GetProcAddress(hDll, "GetAlgorithm");
		if (!getAlgorithmFunc)
		{
			cout << LOAD_DLL_ERROR_STRING << algoFullpath << endl;
			FreeLibrary(hDll); // Make sure to release loaded library, as AlgoLoader doesn't manage it yet
			return NULL;
		}

		// Keep algorithm in list of loaded algos
		_loadedGameAlgos.push_back(std::make_tuple(algoFullpath, hDll, getAlgorithmFunc));

		// Call GetAlgorithm for the specified algorithm, this should create a new instance for that algo type 
		IBattleshipGameAlgo* algo = getAlgorithmFunc();
		if (NULL == algo)
		{
			cout << LOAD_DLL_ERROR_STRING << algoFullpath << endl;
			return NULL;
		}

		return algo;
	}

	const string AlgoLoader::getAlgoPathByIndex(unsigned int index) const
	{
		// Avoid array out of bounds, return an empty string
		if (index >= _availableGameAlgos.size())
		{
			cout << NON_EXISTING_ALGO_ERROR_STRING << endl;
			return "";
		}

		const string algoPath = _availableGameAlgos.at(index);
		
		return algoPath;
	}

	shared_ptr<IBattleshipGameAlgo> AlgoLoader::loadAlgoByLexicalOrder(unsigned int index)
	{
		// Avoid array out of bounds, return NULL
		if (index >= _availableGameAlgos.size())
		{
			cout << NON_EXISTING_ALGO_ERROR_STRING << endl;
			return NULL;
		}

		const string algoPath = _availableGameAlgos.at(index);
		IBattleshipGameAlgo* algo = NULL;

		// Search if algo was already loaded before
		auto it = std::find_if(_loadedGameAlgos.begin(), _loadedGameAlgos.end(),
							   [&algoPath](const AlgoDescriptor& ad) { return std::get<0>(ad) == algoPath; });

		if (it == _loadedGameAlgos.end())
		{	
			// Not loaded before, so load and cache here
			algo = loadAlgorithm(algoPath);
		}
		else
		{	// Loaded before
			// Retrieve algorithm descriptor
			AlgoDescriptor& algoDesc = *it;
			auto getAlgorithmFunc = std::get<2>(algoDesc); // 3rd element holds the function pointer

			// Call GetAlgorithm for the specified algorithm,
			// this should create a new instance for that algo type 
			IBattleshipGameAlgo* algo = getAlgorithmFunc();
			if (NULL == algo)
			{
				cout << LOAD_DLL_ERROR_STRING << std::get<0>(algoDesc) << endl;
				return NULL;
			}
		}

		// Wrap in a smart pointer, so consumers don't have to deal with memory deallocation manually
		return shared_ptr<IBattleshipGameAlgo>(algo);
	}

	bool AlgoLoader::fetchDLLs(const string& path)
	{
		_availableGameAlgos.clear();
		vector<string> foundDlls = IOUtil::listFilesInPath(path, "dll");

		// Scan for dlls in the path, and append the path to their filenames
		for (auto& nextDllFilename : foundDlls)
		{
			string fullFileName = path + "\\" + nextDllFilename;
			_availableGameAlgos.push_back(fullFileName);
		}

		// A game must have 2 algorithms to function properly
		if (_availableGameAlgos.size() < 2)
		{
			cout << MISSING_DLL_ERROR_STRING << path << endl;
		}

		return true;
	}
}