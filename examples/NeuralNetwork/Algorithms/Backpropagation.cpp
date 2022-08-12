#include <ecf/ECF.h>
#include "Backpropagation.h"
#include "../NeuralNetwork.h"

Backpropagation::Backpropagation()
{
	// define algorithm name
	name_ = "Backpropagation";
}

void Backpropagation::registerParameters(StateP state)
{
	registerParameter(state, "learningRate", (voidP) new double(1), ECF::DOUBLE, "the learning rate od BP.");

}


bool Backpropagation::initialize(StateP state)
{
	voidP sptr = getParameterValue(state, "learningRate");
	learningRate_ = *((double*)sptr.get());

	return true;
}


bool Backpropagation::advanceGeneration(StateP state, DemeP deme)
{
	for(uint i = 0; i < deme->size(); i++)
	{
		NeuralNetwork* gen = (NeuralNetwork*) deme->at(i)->getGenotype().get();
		gen->backpropagate(learningRate_);
	}

	// evaluate new individuals
	for(uint i = 0; i < deme->size(); i++){
			evaluate(deme->at(i));
	}

	return true;
}
