#include "AlgoLoader.h"
#include <iostream>
#include <functional>
#include <algorithm>
#include "IBattleshipGameAlgo.h"

using std::cout;
using std::endl;
using std::function;

namespace battleship
{
	AlgoLoader::AlgoLoader()
	{	
	}


	AlgoLoader::~AlgoLoader()
	{
		// Close all the dynamic libs we opened
		for (auto algIter = _availableGameAlgos.begin(); algIter != _availableGameAlgos.end(); ++algIter)
		{
			// Free HINSTANCE loaded, which resides in the 2nd cell of the algo tuple
			FreeLibrary(std::get<1>(*algIter));
		}
	}

	shared_ptr<IBattleshipGameAlgo> AlgoLoader::getAlgoByLexicalOrder(unsigned int index)
	{
		// Avoid array out of bounds, return NULL
		if (index >= _availableGameAlgos.size())
			return NULL;

		// Call GetAlgorithm for the specified algorithm, this should create a new instance for that algo type 
		IBattleshipGameAlgo* algo = std::get<2>(_availableGameAlgos[index])();

		// Wrap in a smart pointer, so consumers don't have to deal with memory deallocation manually
		return shared_ptr<IBattleshipGameAlgo>(algo);
	}

	void AlgoLoader::sortLoadedAlgorithms()
	{
		using LexicalAlgoDescriptorSort = function<bool(const AlgoDescriptor&, const AlgoDescriptor&)>;

		LexicalAlgoDescriptorSort sortFunc = [](const AlgoDescriptor& algo1, const AlgoDescriptor& algo2)
		{
			return std::get<0>(algo1) < std::get<0>(algo1);
		};

		std::sort(_availableGameAlgos.begin(), _availableGameAlgos.end(), sortFunc);
	}

	bool AlgoLoader::loadDLLs(const string& path)
	{
		HANDLE dir;

		WIN32_FIND_DATAA fileData; // Data struct for files information
		vector<HINSTANCE>::iterator itr;

		GetAlgorithmFuncType getAlgorithmFunc;

		// Iterate over *.dll files in path
		string s = "\\*.dll"; // Filter only .dll endings
		dir = FindFirstFileA((path + s).c_str(), &fileData); // Unicode compatible version of FindFirstFile

		// Check if the dir opened successfully
		if (dir != INVALID_HANDLE_VALUE)
		{
			// test each file suffix and set variables as needed
			do
			{
				string fileName = fileData.cFileName;
				string fullFileName = path + "\\" + fileName;
				string algoName = fileName.substr(0, fileName.find("."));

				// Load dynamic library
				// Again using Unicode compatible version of LoadLibrary
				HINSTANCE hDll = LoadLibraryA(fullFileName.c_str());
				if (!hDll)
				{
					cout << "Cannot load dll: " << fullFileName << endl;
					return false;
				}

				// Get function pointer
				getAlgorithmFunc = (GetAlgorithmFuncType)GetProcAddress(hDll, "GetAlgorithm");
				if (!getAlgorithmFunc)
				{
					
					cout << "Algorithm initialization failed for dll: " << fullFileName << endl;
					return false;
				}

				_availableGameAlgos.push_back(make_tuple(algoName, hDll, getAlgorithmFunc));

			} while (FindNextFileA(dir, &fileData)); // Notice: Unicode compatible version of FindNextFile
		}

		sortLoadedAlgorithms();

		return true;
	}
}