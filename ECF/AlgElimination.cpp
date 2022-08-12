#include "ECF_base.h"
#include "AlgElimination.h"
#include "ECF_macro.h"


Elimination :: Elimination()
{
	name_ = "Elimination";

	selFitPropOp = static_cast<SelFitnessProportionalOpP> (new SelFitnessProportionalOp);
	selRandomOp = static_cast<SelRandomOpP> (new SelRandomOp);
	selBestOp = static_cast<SelBestOpP> (new SelBestOp);
}


void Elimination :: registerParameters(StateP state)
{
	registerParameter(state, "gengap", (voidP) new double(0.6), ECF::DOUBLE,
		"generation gap (percentage of population to be eliminated)");
	registerParameter(state, "selpressure", (voidP) new double(10), ECF::DOUBLE,
		"selection pressure: how much is the worst individual 'worse' than the best");
}


bool Elimination :: initialize(StateP state)
{
	selFitPropOp->initialize(state);
	selRandomOp->initialize(state);
	selBestOp->initialize(state);

	voidP genGapP = getParameterValue(state, "gengap");
	genGap_ = *((double*) genGapP.get());

	if(genGap_ <= 0 || genGap_ > 1) {
		ECF_LOG_ERROR(state, "Error: generation gap parameter in Elimination algorithm must be in <0, 1]!");
		throw "";
	}

	voidP selPressP = getParameterValue(state, "selpressure");
	selPressure_ = *((double*) selPressP.get());

	// selection chooses worse individuals
	selFitPropOp->setSelPressure(1./selPressure_);

	return true;
}


bool Elimination :: advanceGeneration(StateP state, DemeP deme)
{
	// elitism: copy current best individual
	IndividualP best = selBestOp->select(*deme);
	best = copy(best);

	// copy pointers
	std::vector<IndividualP> newGen;
	for(uint i = 0; i < deme->size(); i++)
		newGen.push_back(deme->at(i));

	// eliminate genGap_ worse individuals
	uint generationGap = (uint) (genGap_ * deme->size());
	for(uint i = 0; i < generationGap; i++) {
		IndividualP victim = selFitPropOp->select(newGen);
		removeFrom(victim, newGen);
	}

	// make genGap_ new ones
	for(uint i = 0; i < generationGap; i++) {
		IndividualP parent1 = selRandomOp->select(*deme);
		IndividualP parent2 = selRandomOp->select(*deme);
		IndividualP child = copy(parent1);
		mate(parent1, parent2, child);
		newGen.push_back(child);
	}

	// replace
	for(uint i = 0; i < deme->size(); i++)
		replaceWith(deme->at(i), newGen[i]);

	// mutate all
	mutate(*deme);

	// evaluate new individuals
	for(uint i = 0; i < deme->size(); i++)
		if(!deme->at(i)->fitness->isValid()) {
			evaluate(deme->at(i));
		}

	// elitism: preserve best individual
	IndividualP random = selFitPropOp->select(*deme);
	if(best->fitness->isBetterThan(random->fitness))
		replaceWith(random, best);

	return true;
}
