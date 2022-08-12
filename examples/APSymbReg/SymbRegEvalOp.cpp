#include <cmath>
#include <ECF/ECF.h>
#include "SymbRegEvalOp.h"


// called only once, before the evolution – generates training data
bool SymbRegEvalOp::initialize(StateP state)
{
	nSamples = 10;
	double x = -10;
	for(uint i = 0; i < nSamples; i++) {
		domain.push_back(x);
		codomain.push_back(x + sin(x));
		x += 2;
	}

	useAPGenotype = false;
	GenotypeP activeGenotype = state->getGenotypes()[0];
	if(activeGenotype->getName() == "APGenotype")
		useAPGenotype = true;

	return true;
}


FitnessP SymbRegEvalOp::evaluate(IndividualP individual)
{
	// we try to minimize the function value, so we use FitnessMin fitness (for minimization problems)
	FitnessP fitness (new FitnessMin);

	// get the genotype we defined in the configuration file
	Tree::APGenotype* apg = (Tree::APGenotype*) individual->getGenotype().get();

	Tree::Tree* tree;
	if (useAPGenotype == true)
		tree = (Tree::Tree*) ((Tree::APGenotype*)individual->getGenotype().get())->convertToPhenotype();
	else
		tree = (Tree::Tree*) individual->getGenotype().get();

	double value = 0;
	for(uint i = 0; i < nSamples; i++) {
		// for each test data instance, the x value (domain) must be set
		if(useAPGenotype == true)
			apg->setTerminalValue(tree, "X", &domain[i]);
		else
			tree->setTerminalValue("X", &domain[i]);
		// get the y value of the current tree
		double result;
		tree->execute(&result);
		// add the difference
		value += fabs(codomain[i] - result);
	}
	fitness->setValue(value);

	return fitness;
}
