#include "CartesianMutNonSilent.h"
#include "Cartesian_c.h"
#include <cmath>
#include <vector>
//#include <algorithm>

namespace Cartesian
{
	void CartesianMutNonSilent::registerParameters(StateP state)
	{
		myGenotype_->registerParameter(state, "mut.nonsilent", (voidP) new double(0), ECF::DOUBLE);
	}


	bool CartesianMutNonSilent::initialize(StateP state)
	{
		voidP sptr = myGenotype_->getParameterValue(state, "mut.nonsilent");
		probability_ = *((double*)sptr.get());
		return true;
	}


	bool CartesianMutNonSilent::mutate(GenotypeP gene)
	{
		Cartesian* cart = (Cartesian*)(gene.get());
		RandomizerP randP = cart->state_->getRandomizer();
		const uint nOutputs = cart->nOutputs_;
		const uint nRows = cart->nRows_;
		const uint nCols = cart->nCols_;
		const uint nInputs = cart->nInputs_;
		const uint nLevelsBack = cart->nLevelsBack_;

		std::vector<uint> activeNodes;
		cart->getActiveFunctionNodes(activeNodes);

		// if all function nodes are active
		if (activeNodes.size() == (nRows * nCols))
			return false;

		std::vector<uint> silentNodes;
		uint iActive = 0;
		for(uint i = nInputs; i < cart->nodes_.size(); i++)
			if (iActive < activeNodes.size() && activeNodes[iActive] == i)
				iActive++;
			else
				silentNodes.push_back(i);

		// choose a silent node that should be mutated into active
		uint silentNodeIndex = randP->getRandomInteger((uint) silentNodes.size());
		uint silentNode = silentNodes[silentNodeIndex];
		uint silentNodeColumn = (silentNode - nInputs) / nRows;

		// index of first node in the next column
		uint nextColumnNode = (silentNodeColumn + 1) * nRows + nInputs;

		// find all active nodes that could have silentNode as an input -> validNodes
		// validNodes also contain output indexes
		std::vector<uint> validNodes;
		for (uint node : activeNodes) {
			uint nodeColumn = (node - nInputs) / nRows;
			if (node >= nextColumnNode && (nodeColumn - silentNodeColumn) <= nLevelsBack)
				validNodes.push_back(node);
		}

		// add indexes that represent outputs
		for (uint i = 0; i < nOutputs; i++)
			validNodes.push_back(i + (uint) cart->nodes_.size());

		uint mutateNodeIndex = randP->getRandomInteger((uint) validNodes.size());
		uint mutateNode = validNodes[mutateNodeIndex];

		// if mutateNode is output
		if (mutateNode >= cart->nodes_.size()) {
			cart->outputs_[mutateNode - cart->nodes_.size()] = silentNode;
		} else {
			// choose random input of the node that will be mutated
			uint input = randP->getRandomInteger(cart->nodes_[mutateNode].primitive_->getNumberOfArguments());
			cart->nodes_[mutateNode].arguments_[input] = silentNode;
		}

		return true;
	}

}
