#pragma once

#include <memory>
#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "IBattleshipGameAlgo.h"

using std::shared_ptr;
using std::vector;
using std::unordered_map;
using std::string;

namespace battleship
{
	/** Used to create new IBattleshipGameAlgo instances from loaded DLLs.
	 */
	class AlgoLoader
	{
	public:
		/** Constructs a new instance of AlgoLoader, used to create new IBattleshipGameAlgo instances
		 *	from available DLLs.
		 */
		AlgoLoader(const string& path);

		/** Releases all dynamic libraries loaded by this algorithm loader */
		virtual ~AlgoLoader();

		AlgoLoader(AlgoLoader const&) = delete;						  // Disable copying
		AlgoLoader& operator=(AlgoLoader const&) = delete;			  // Disable copying (assignment)
		AlgoLoader(AlgoLoader&& other) noexcept = delete;			  // Disable moving
		AlgoLoader& operator= (AlgoLoader&& other) noexcept = delete; // Disable moving (assignment)

		/** Get list of algorithms available for loading (their dll is present in the game dir) */
		const vector<string>& availableGameAlgos() const;

		/** Get list of algorithm whose dll was successfully loaded */
		const vector<string>& loadedGameAlgos() const;

		/** Creates a new instance of the algorithm in the given path.
		 *  This method assumes algoPath was loaded successfully by this object.
		 *  (algoPath should appear in "loadedAlgos()")
		 *  This method is thread safe.
		 */
		shared_ptr<IBattleshipGameAlgo> requestAlgo(const string& algoPath) const;

	private:

		/** Typedef for object creating new IBattleshipGameAlgo objects from Dlls */
		using GetAlgorithmFuncType = IBattleshipGameAlgo *(*)();

		/** Descriptor for IBattleshipGameAlgo available for loading.
		 *	This is essentially all the information available on an algorithm we can load.
		 */
		struct AlgoDescriptor
		{
			string path;
			HINSTANCE dll;
			GetAlgorithmFuncType algoFunc;

			AlgoDescriptor(string& aPath, HINSTANCE aDll, GetAlgorithmFuncType aAlgoFunc)
			{
				path = aPath;
				dll = aDll;
				algoFunc = aAlgoFunc;
			}
		};

		/** Vector of available algorithms for loading: <Algorithm name> */
		vector<string> _availableGameAlgos;

		/** Vector of loaded algorithm names */
		vector<string> _loadedGameAlgoNames;

		/** Vector of loaded algorithms: <Algorithm name, dll handle, GetAlgorithm function ptr> */
		vector<shared_ptr<AlgoDescriptor>> _loadedGameAlgos;

		/** Loads the algorithm's DLL in the given path */
		void loadAlgorithm(const string& algoFullpath);

		/** Fetches names for all algorithms available in the given path.
	 	 *	(populates the AlgoLoad with available dlls for loading)
		 */
		void fetchDLLs(const string& path);
	};

}
