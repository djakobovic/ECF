#include "CartesianMutOnePointActive.h"
#include "Cartesian_c.h"
#include "FunctionSet.h"
#include <cmath>
#include <vector>
#include <algorithm>

namespace Cartesian {

	void CartesianMutOnePointActive::registerParameters(StateP state)
	{
		myGenotype_->registerParameter(state, "mut.onepointactive", (voidP) new double(0), ECF::DOUBLE);
	}


	bool CartesianMutOnePointActive::initialize(StateP state)
	{
		voidP sptr = myGenotype_->getParameterValue(state, "mut.onepointactive");
		probability_ = *((double*)sptr.get());
		return true;
	}

	bool CartesianMutOnePointActive::mutate(GenotypeP gene)
	{
		Cartesian* cart = (Cartesian*)(gene.get());
		RandomizerP rand = cart->state_->getRandomizer();
		const uint size = cart->getGenomeSize();
		const uint nConstants = cart->nConstants_;
		const uint nOutputs = cart->nOutputs_;
		const uint nRows = cart->nRows_;
		const uint nCols = cart->nCols_;
		const uint nFunctions = cart->nFunctions_;
		const uint nInputs = cart->nInputs_;
		uint randomGeneIndex;

		// select random active function or output node
		std::vector<uint> validNodes;
		cart->getActiveFunctionNodes(validNodes);
		for (uint i = 0; i < nOutputs; i++)
			validNodes.push_back((uint) cart->nodes_.size() + i);
		randomGeneIndex = validNodes[rand->getRandomInteger((uint) validNodes.size())];

		if (randomGeneIndex < (size + nInputs - nOutputs)) {
			// Function node
			uint iColumn = (randomGeneIndex - nInputs) / nRows;
			if (rand->getRandomInteger(2) == 0) {
				// mutate function and arguments
				int functionID = rand->getRandomInteger(nFunctions);
				Function* func = cart->functionSet_->vFunctions[functionID].get();
				cart->nodes_[randomGeneIndex].setPrimitive(cart->functionSet_->vFunctions[functionID]);
				// generate random arguments
				cart->nodes_[randomGeneIndex].arguments_.resize(func->getNumberOfArguments());
				for (uint k = 0; k < func->getNumberOfArguments(); k++) {
					uint iArgument = cart->randomNodeInputConnection(iColumn);
					cart->nodes_[randomGeneIndex].arguments_[k] = iArgument;
				}
			} else {
				// mutate one of the arguments
				uint iArg = rand->getRandomInteger(cart->nodes_[randomGeneIndex].primitive_->getNumberOfArguments());
				cart->nodes_[randomGeneIndex].arguments_[iArg] = cart->randomNodeInputConnection(iColumn);
			}
		} else {
			// Output gene
			cart->outputs_[randomGeneIndex - cart->nodes_.size()] = cart->randomOutputConnection();
		}

		//std::cout << randomGeneIndex << "\n";

		return true;
	}

}