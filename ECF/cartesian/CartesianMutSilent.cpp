#include "CartesianMutSilent.h"
#include "Cartesian_c.h"
#include <cmath>
#include <vector>
#include <algorithm>

namespace Cartesian
{
	static void addRecursivelyActiveNodes(Cartesian* cart, std::vector<uint>& activeNodes, uint node)
	{
		uint startIndex = (node - cart->nInputs) * (cart->maxArity + 1);
		for (uint nodeInput = 1; nodeInput <= cart->maxArity; nodeInput++) {
			uint activeNode = cart->at(startIndex + nodeInput);
			if (activeNode >= cart->nInputs &&
				find(activeNodes.begin(), activeNodes.end(), activeNode) == activeNodes.end())
			{
				activeNodes.push_back(activeNode);
				addRecursivelyActiveNodes(cart, activeNodes, activeNode);
			}
		}
	}

	static std::vector<uint> getActiveNodes(Cartesian* cart)
	{
		const uint size = cart->getGenomeSize();
		std::vector<uint> activeNodes;

		// Add output nodes into active nodes
		for (uint output = size - cart->nOutputs; output < size; output++) {
			uint activeNode = cart->at(output);
			// 'activeNode' can be one of the inputs (variables or constants)
			// If 'activeNode' is not input and if activeNodes doesn't already contain that node
			if (activeNode >= cart->nInputs &&
				find(activeNodes.begin(), activeNodes.end(), activeNode) == activeNodes.end())
			{
				activeNodes.push_back(activeNode);
			}
		}

		std::vector<uint> nodes;
		for (uint node : activeNodes) {
			addRecursivelyActiveNodes(cart, nodes, node);
		}

		activeNodes.insert(activeNodes.end(), nodes.begin(), nodes.end());
		std::sort(activeNodes.begin(), activeNodes.end());
		return activeNodes;
	}

	void CartesianMutSilent::registerParameters(StateP state)
	{
		myGenotype_->registerParameter(state, "mut.nonsilent", (voidP) new double(0), ECF::DOUBLE);
	}


	bool CartesianMutSilent::initialize(StateP state)
	{
		voidP sptr = myGenotype_->getParameterValue(state, "mut.nonsilent");
		probability_ = *((double*)sptr.get());
		return true;
	}


	bool CartesianMutSilent::mutate(GenotypeP gene)
	{
		Cartesian* cart = (Cartesian*)(gene.get());
		RandomizerP randP = cart->state_->getRandomizer();
		const uint nConstants = cart->nConstants;
		const uint nOutputs = cart->nOutputs;
		const uint nVariables = cart->nVariables;
		const uint nRows = cart->nRows;
		const uint nCols = cart->nCols;
		const uint nLevelsBack = cart->nLevelsBack;
		const uint maxArity = cart->maxArity;
		const uint nInputs = cart->nInputs;

		std::vector<uint> activeNodes = getActiveNodes(cart);

		// If all nodes are active
		if (activeNodes.size() == nRows * nCols)
			return true;

		std::vector<uint> allNodes;
		for (uint i = 0; i < nRows * nCols; i++) {
			allNodes.push_back(i + nInputs);
		}

		std::vector<uint> silentNodes;
		std::sort(activeNodes.begin(), activeNodes.end());
		set_difference(allNodes.begin(), allNodes.end(), activeNodes.begin(), activeNodes.end(), inserter(silentNodes, silentNodes.begin()));

		// Choose silent node that will be mutated
		uint silentNodeIndex = randP->getRandomInteger(0, silentNodes.size() - 1);
		uint silentNode = silentNodes.at(silentNodeIndex);

		uint nodeColumn = (silentNode - nInputs) / nRows;
		int minColumn = nodeColumn - nLevelsBack;
		uint lowerBound = nInputs + minColumn * nRows;
		if (minColumn < 0) lowerBound = 0;
		uint upperBound = nInputs + nodeColumn * nRows;
		uint newSilentNodeInput = randP->getRandomInteger(lowerBound, upperBound - 1);

		// Choose random input of silent node that will be mutated
		uint inputIndex = randP->getRandomInteger(1, maxArity);
		// Find the right index of node in genotype
		uint genotypeIndex = (silentNode - nInputs) * (maxArity + 1);

		cart->at(genotypeIndex + inputIndex) = newSilentNodeInput;

		return true;
	}

}