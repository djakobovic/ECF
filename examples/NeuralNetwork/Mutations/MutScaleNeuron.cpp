#include <cmath>
#include <ecf/ECF.h>
#include <iostream>
#include <random>
#include "MutScaleNeuron.h"
#include "../NeuralNetwork.h"


void MutScaleNeuron::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "mut.scaleneuron", (voidP) new double(0), ECF::DOUBLE);
}

bool MutScaleNeuron::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "mut.scaleneuron");
	probability_ = *((double*)sptr.get());

	return true;
}


bool MutScaleNeuron::mutate(GenotypeP gene)
{
	NeuralNetwork* FP = (NeuralNetwork*)(gene.get());

	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_real_distribution<> distrScale(0.5, 1.5);

	vector <uint> structure = FP->getLayerStructure();
	uint noLayers = FP->getNoLayers();

	uint chosenLayer = state_->getRandomizer()->getRandomInteger(-5, 5) % (noLayers - 2 + 1) + 2;
	uint chosenNeuron = state_->getRandomizer()->getRandomInteger(-5, 5) % (structure[chosenLayer - 1]) + 1;
	
	vector <double> weights;
	FP->getWeights(chosenLayer, chosenNeuron, weights);

	for (uint i = 0; i<weights.size(); i++) {
		weights[i] *= distrScale(eng);
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
