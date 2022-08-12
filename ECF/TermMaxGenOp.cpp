#include "ECF_base.h"
#include "TermMaxGenOp.h"


void TermMaxGenOp::registerParameters(StateP state)
{
	uint *maxgen = new uint(0);
	state->getRegistry()->registerEntry("term.maxgen", (voidP) maxgen, ECF::UINT,
		"max number of generations (default: none)");
}


bool TermMaxGenOp::initialize(StateP state)
{
	voidP sptr = state->getRegistry()->getEntry("term.maxgen");
	nGenerations_ = *((uint*) sptr.get());

	if(state->getRegistry()->isModified("term.maxgen"))
		return true;

	return false;
}


bool TermMaxGenOp::operate(StateP state)
{
	if(state->getGenerationNo() >= nGenerations_) {
		state->setTerminateCond();
		ECF_LOG(state, 1, "Termination: max generation reached (" + uint2str(nGenerations_) + ")");
	}

	return true;
}