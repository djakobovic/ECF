#include "ECF_base.h"
#include "AlgRouletteWheel.h"
#include "ECF_macro.h"


RouletteWheel :: RouletteWheel()
{
	name_ = "RouletteWheel";

	selFitPropOp = static_cast<SelFitnessProportionalOpP> (new SelFitnessProportionalOp);
	selRandomOp = static_cast<SelRandomOpP> (new SelRandomOp);
	selBestOp = static_cast<SelBestOpP> (new SelBestOp);
}


void RouletteWheel :: registerParameters(StateP state)
{
	registerParameter(state, "crxprob", (voidP) new double(0.5), ECF::DOUBLE, "crossover rate");
	registerParameter(state, "selpressure", (voidP) new double(10), ECF::DOUBLE, 
		"selection pressure: how much is the best individual 'better' than the worst");
}


bool RouletteWheel :: initialize(StateP state)
{
	selFitPropOp->initialize(state);
	selRandomOp->initialize(state);
	selBestOp->initialize(state);

	voidP crRateP = getParameterValue(state, "crxprob");
	crxRate_ = *((double*) crRateP.get());

	voidP selPressP = getParameterValue(state, "selpressure");
	selPressure_ = *((double*) selPressP.get());

	selFitPropOp->setSelPressure(selPressure_);

	return true;
}


bool RouletteWheel :: advanceGeneration(StateP state, DemeP deme)
{
	// elitism: copy current best individual
	IndividualP best = selBestOp->select(*deme);
	best = copy(best);

	// select individuals
	std::vector<IndividualP> wheel;
	wheel = selFitPropOp->selectMany(*deme, (uint) deme->size());

	// copy selected to new population
	for(uint i = 0; i < wheel.size(); ++i) 
		wheel[i] = copy(wheel[i]);

	// replace old population
	for(uint i = 0; i < deme->size(); i++)
		replaceWith((*deme)[i], wheel[i]);

	ECF_LOG(state, 5, "Selected individuals:");
	for(uint i = 0; i < deme->size(); i++){
		ECF_LOG(state, 5, dbl2str(deme->at(i)->fitness->getValue()));
	}

	// determine the number of crx operations
	uint noCrx = (int)(deme->size() * crxRate_ /2);

	// perform crossover
	for(uint i = 0; i < noCrx; i++){

		// select parents
		IndividualP parent1 = selRandomOp->select(*deme);
		IndividualP parent2 = selRandomOp->select(*deme);
		ECF_LOG(state, 5, "Parents: " + dbl2str(parent1->fitness->getValue()) + ", " + dbl2str(parent2->fitness->getValue()));

		// create children
		IndividualP child1 = copy(parent1);
		IndividualP child2 = copy(parent2);

		// perform crx operations
		mate(parent1, parent2, child1);
		mate(parent1, parent2, child2);

		// replace parents with children
		replaceWith(parent1, child1);
		replaceWith(parent2, child2);
	}

	// perform mutation on whole population
	mutate(*deme);

	// evaluate new individuals
	for(uint i = 0; i < deme->size(); i++)
		if(!deme->at(i)->fitness->isValid()) {
			evaluate(deme->at(i));
		}

	// elitism: preserve best individual
	IndividualP random = selRandomOp->select(*deme);
	if(best->fitness->isBetterThan(random->fitness))
		replaceWith(random, best);

	return true;
}
