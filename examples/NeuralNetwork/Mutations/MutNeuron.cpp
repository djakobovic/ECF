#include <ecf/ECF.h>
#include <cmath>
// #include "MutNeuron.h"
#include "../NeuralNetwork.h"

// {

void MutNeuron::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "mut.neuron", (voidP) new double(0), ECF::DOUBLE);
}

bool MutNeuron::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "mut.neuron");
	probability_ = *((double*)sptr.get());

	return true;
}

bool MutNeuron::mutate(GenotypeP gene)
{
	NeuralNetwork* FP = (NeuralNetwork*) (gene.get());

    vector <uint> structure = FP->getLayerStructure();
	uint noLayers = FP->getNoLayers();

	uint chosenLayer = state_->getRandomizer()->getRandomInteger(-5, 5) % (noLayers-2+1) + 2;
    uint chosenNeuron = state_->getRandomizer()->getRandomInteger(-5, 5) % (structure[chosenLayer-1]) + 1;

    vector <double> weights;
    FP->getWeights(chosenLayer, chosenNeuron, weights);
    for (uint i=0; i<weights.size(); i++) {
        weights[i] = weights[i] + (state_->getRandomizer()->getRandomDouble() / (FP->getUBound() - FP->getLBound() +1) + FP->getLBound());
        if (weights[i] > FP->getUBound()) {
            weights[i] = FP->getUBound();
        }
        if (weights[i] < FP->getLBound()) {
            weights[i] = FP->getLBound();
        }
    }
    FP->setWeights(chosenLayer, chosenNeuron, weights);

	return true;
}

// }
