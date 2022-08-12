#include <ecf/ECF.h>
#include "FunctionMinEvalOp.h"
#include "NeuralNetwork.h"


void FunctionMinEvalOp::registerParameters(StateP state)
{
}


bool FunctionMinEvalOp::initialize(StateP state)
{
	return true;
}


FitnessP FunctionMinEvalOp::evaluate(IndividualP individual)
{
	NeuralNetwork* gen = (NeuralNetwork*) individual->getGenotype().get();

	// evaluation creates a new fitness object using a smart pointer
	// in our case, we try to minimize the function value, so we use FitnessMin fitness (for minimization problems)
	FitnessP fitness (new FitnessMin);

	double fit = gen->getTotalError();
	Errors_.push_back(fit);
	fitness->setValue(fit);
	return fitness;
}
