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

		/** Loads all algorithms available in the given path */
		bool loadDLLs(const string& path);

		/** Returns the algorithm loaded in lexicographical order by index */
		shared_ptr<IBattleshipGameAlgo> getAlgoByLexicalOrder(unsigned int index);

	private:

		/** Typedef for object creating new IBattleshipGameAlgo objects from Dlls */
		using GetAlgorithmFuncType = IBattleshipGameAlgo *(*)();

		/** Typedef of descriptor for IBattleshipGameAlgo available for loading.
		 *	This is essentially all the information available on an algorithm we can load.
		 */
		using AlgoDescriptor = tuple<string, HINSTANCE, GetAlgorithmFuncType>;

		/** Vector of available algorithms: <Algorithm name, dll handle, GetAlgorithm function ptr> */
		vector<AlgoDescriptor> _availableGameAlgos;

		/** Sorts loaded algorithms lexicographically by algorithm name */
		void sortLoadedAlgorithms();
	};

}
