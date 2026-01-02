#include "CartesianMutNewParameterLess.h"
#include "Cartesian_c.h"
#include <cmath>
#include <vector>
#include <algorithm>

namespace Cartesian {

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


	void CartesianMutNewParameterLess::registerParameters(StateP state)
	{
		myGenotype_->registerParameter(state, "mut.newparameterless", (voidP) new double(0), ECF::DOUBLE);
	}


	bool CartesianMutNewParameterLess::initialize(StateP state)
	{
		voidP sptr = myGenotype_->getParameterValue(state, "mut.newparameterless");
		probability_ = *((double*)sptr.get());
		return true;
	}

	bool CartesianMutNewParameterLess::mutate(GenotypeP gene)
	{
		Cartesian* cart = (Cartesian*)(gene.get());
		RandomizerP randP = cart->state_->getRandomizer();
		const uint size = cart->getGenomeSize();
		const uint nConstants = cart->nConstants;
		const uint nOutputs = cart->nOutputs;
		const uint nVariables = cart->nVariables;
		const uint rows = cart->nRows;
		const uint cols = cart->nCols;
		const uint levelsBack = cart->nLevelsBack;
		const uint nFunctions = cart->nFunctions;
		const uint maxArity = cart->maxArity;
		const uint nInputs = cart->nInputs;
		uint randomGeneIndex;
		uint newGene = 0;
		bool geneIsActive = false;
		bool isOutputGene;

		do {
			randomGeneIndex = randP->getRandomInteger(0, size - 1);
			isOutputGene = false;

			if (randomGeneIndex < size - nOutputs) {
				if (randomGeneIndex % (maxArity + 1) == 0) {
					// Function gene
					newGene = randP->getRandomInteger(0, nFunctions - 1);
				}
				else {
					// Input gene
					uint randomNode = randomGeneIndex / (maxArity + 1);
					uint nodeColumn = randomNode / rows;
					int minColumn = nodeColumn - levelsBack;
					uint lowerBound = nInputs + minColumn * rows;
					if (minColumn < 0) lowerBound = 0;
					uint upperBound = nInputs + nodeColumn * rows;
					newGene = randP->getRandomInteger(lowerBound, upperBound - 1);
				}
			}
			else {
				// Output gene
				uint upperBound = nInputs + rows * cols;
				uint lowerBound = 0;
				newGene = randP->getRandomInteger(lowerBound, upperBound - 1);
				isOutputGene = true;
			}

			cart->at(randomGeneIndex) = newGene;

			// Check if gene is active
			if (isOutputGene) {
				// If gene is output, then it's active
				geneIsActive = true;
			}
			else {
				// Check if node is active whose gene is mutated
				uint node = randomGeneIndex / (maxArity + 1) + nInputs;
				std::vector<uint> activeNodes = getActiveNodes(cart);
				if ((find(activeNodes.begin(), activeNodes.end(), node)) != activeNodes.end()) {
					geneIsActive = true;
				}
			}

		} while (!geneIsActive);

		return true;
	}

}