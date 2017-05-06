#pragma once

#include <memory>
#include <windows.h>
#include <vector>
#include <tuple>
#include <string>
#include "IBattleshipGameAlgo.h"

using std::shared_ptr;
using std::vector;
using std::tuple;
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
		AlgoLoader();

		/** Releases all dynamic libraries loaded by this algorithm loader */
		virtual ~AlgoLoader();

		AlgoLoader(AlgoLoader const&) = delete;						  // Disable copying
		AlgoLoader& operator=(AlgoLoader const&) = delete;			  // Disable copying (assignment)
		AlgoLoader(AlgoLoader&& other) noexcept = delete;			  // Disable moving
		AlgoLoader& operator= (AlgoLoader&& other) noexcept = delete; // Disable moving (assignment)

		/** Fetches names for all algorithms available in the given path.
		 *	(populates the AlgoLoad with available dlls for loading)
		 */
		bool fetchDLLs(const string& path);

		/** Loads & returns the algorithm available in lexicographical order by index.
		 *	Algorithms can only be loaded after their dll have been fetched.
		 */
		shared_ptr<IBattleshipGameAlgo> loadAlgoByLexicalOrder(unsigned int index);

		/** Returns the algorithm name at the given index (algorithms are sorted in a list)
		 *	If an error occurs, an empty string is returned.
		 */
		const string getAlgoPathByIndex(unsigned int index) const;

	private:

		// Error message strings
		static const string LOAD_DLL_ERROR_STRING;
		static const string MISSING_DLL_ERROR_STRING;
		static const string NON_EXISTING_ALGO_ERROR_STRING;

		/** Typedef for object creating new IBattleshipGameAlgo objects from Dlls */
		using GetAlgorithmFuncType = IBattleshipGameAlgo *(*)();

		/** Typedef of descriptor for IBattleshipGameAlgo available for loading.
		 *	This is essentially all the information available on an algorithm we can load.
		 */
		using AlgoDescriptor = tuple<string, HINSTANCE, GetAlgorithmFuncType>;

		/** Vector of available algorithms for loading: <Algorithm name> */
		vector<string> _availableGameAlgos;

		/** Vector of loaded algorithms: <Algorithm name, dll handle, GetAlgorithm function ptr> */
		vector<AlgoDescriptor> _loadedGameAlgos;

		/** Loads the algorithm in the given path. */
		IBattleshipGameAlgo* AlgoLoader::loadAlgorithm(const string& algoFullpath);
	};

}
