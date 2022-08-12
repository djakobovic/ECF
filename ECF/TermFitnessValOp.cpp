#include "ECF_base.h"
#include "TermFitnessValOp.h"


void TermFitnessValOp::registerParameters(StateP state)
{
	double *value = new double(0);
	state->getRegistry()->registerEntry("term.fitnessval", (voidP) value, ECF::DOUBLE,
		"fitness value for termination (default: none)");
}


bool TermFitnessValOp::initialize(StateP state)
{
	if(!state->getRegistry()->isModified("term.fitnessval"))
		return false;

	voidP sptr = state->getRegistry()->getEntry("term.fitnessval");
	termValue_ = *((double*) sptr.get());

	return true;
}


bool TermFitnessValOp::operate(StateP state)
{
	double minFitness = state->getStats()->getFitnessMin();
	double maxFitness = state->getStats()->getFitnessMax();

	if(termValue_ >= minFitness && termValue_ <= maxFitness) {
		state->setTerminateCond();
		ECF_LOG(state, 1, "Termination: fitness value (" + dbl2str(termValue_) + ") reached");
	}

	return true;
}