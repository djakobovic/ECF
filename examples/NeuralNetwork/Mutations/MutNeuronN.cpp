#include <ecf/ECF.h>
#include <cmath>
#include "MutNeuronN.h"
#include "../NeuralNetwork.h"


void MutNeuronN::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "mut.neuronN", (voidP) new double(0), ECF::DOUBLE);
}


bool MutNeuronN::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "mut.neuronN");
	probability_ = *((double*)sptr.get());

	return true;
}


bool MutNeuronN::mutate(GenotypeP gene)
{
	NeuralNetwork* nn = (NeuralNetwork*) (gene.get());

	vector <uint> structure = nn->getLayerStructure();
	uint noLayers = nn->getNoLayers();

	uint noNeurons = 0;
	for (uint i = 1; i < structure.size(); i++) {
		noNeurons += structure[i];
	}

	uint n = state_->getRandomizer()->getRandomInteger(noNeurons / 2) + 1;

	for (uint j = 0; j < n; j++) {
		uint chosenLayer = state_->getRandomizer()->getRandomInteger(1, noLayers - 1);
		uint chosenNeuron = state_->getRandomizer()->getRandomInteger(structure[chosenLayer]);

		vector <double> weights (structure[chosenLayer] + 1);
		nn->getWeights(chosenLayer + 1, chosenNeuron + 1, weights);
		for (uint i = 0; i < weights.size(); i++) {
			weights[i] = weights[i] + (state_->getRandomizer()->getRandomDouble() / (nn->getUBound() - nn->getLBound() + 1) + nn->getLBound());
			if (weights[i] > nn->getUBound()) {
				weights[i] = nn->getUBound();
			}
			if (weights[i] < nn->getLBound()) {
				weights[i] = nn->getLBound();
			}
		}
		nn->setWeights(chosenLayer + 1, chosenNeuron + 1, weights);
	}

	return true;
}
