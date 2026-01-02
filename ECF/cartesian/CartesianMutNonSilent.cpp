#include "CartesianMutNonSilent.h"
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
		for (uint outputIndex = size - cart->nOutputs; outputIndex < size; outputIndex++) {
			uint activeNode = cart->at(outputIndex);
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
		if (activeNodes.size() >= nRows * nCols)
			return true;

		std::vector<uint> allNodes;
		for (uint i = 0; i < nRows * nCols; i++) {
			allNodes.push_back(i + nInputs);
		}

		std::vector<uint> silentNodes;
		set_difference(allNodes.begin(), allNodes.end(), activeNodes.begin(), activeNodes.end(), inserter(silentNodes, silentNodes.begin()));

		// Choose silent node that will be mutated into active
		uint silentNodeIndex = randP->getRandomInteger(0, silentNodes.size() - 1);
		uint silentNode = silentNodes.at(silentNodeIndex);

		// Find all silent and active nodes that could have silentNode as an input := validNodes
		// validNodes also contain output indexes
		std::vector<uint> validNodes;
		uint largestNodeIndex = nInputs + nRows * nCols;
		uint nodeColmn = (silentNode - nInputs) / nRows;
		uint minColmn = nodeColmn + 1;

		// If minColumn is not out of range, add those nodes in validNodes
		if (minColmn < nCols)
		{
			uint startIndex = minColmn * nRows + nInputs;
			uint endIndex = startIndex + nLevelsBack * nRows;
			if (endIndex > largestNodeIndex)
				endIndex = largestNodeIndex;

			for (uint node = startIndex; node < endIndex; node++)
				validNodes.push_back(node);

		}

		// Add indexes that represent outputs
		for (uint i = 0; i < nOutputs; i++)
			validNodes.push_back(i + largestNodeIndex);

		// Throw away all valid nodes that are silent
		// Active nodes that are valid for mutation := mutateNodes
		std::vector<uint> mutateNodes;
		set_difference(validNodes.begin(), validNodes.end(), silentNodes.begin(), silentNodes.end(), inserter(mutateNodes, mutateNodes.begin()));
		uint mutateNodeIndex = randP->getRandomInteger(0, mutateNodes.size() - 1);
		uint mutateNode = mutateNodes.at(mutateNodeIndex);

		// If mutateNode is output
		if (mutateNode >= largestNodeIndex) {
			// Find beginning index of outputs in genotype
			uint genotypeIndex = nRows * nCols * (maxArity + 1);
			// Get index of output that will be changed
			uint output = mutateNode - largestNodeIndex;
			cart->at(genotypeIndex + output) = silentNode;
		}
		else {
			// Find place of the node in genotype
			uint genotypeIndex = (mutateNode - nInputs) * (maxArity + 1);
			// Choose random input of the node that will be mutated
			uint input = randP->getRandomInteger(1, maxArity);
			cart->at(genotypeIndex + input) = silentNode;
		}

		return true;
	}

}
