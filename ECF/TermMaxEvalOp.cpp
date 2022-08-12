#include "ECF_base.h"
#include "TermMaxEvalOp.h"


void TermMaxEvalOp::registerParameters(StateP state)
{
	uint *eval = new uint(0);
	state->getRegistry()->registerEntry("term.eval", (voidP) eval, ECF::UINT,
		"max number of fitness evaluations (default: none)");
}


bool TermMaxEvalOp::initialize(StateP state)
{
	voidP sptr = state->getRegistry()->getEntry("term.eval");
	nEvaluations_ = *((uint*) sptr.get());

	if(state->getRegistry()->isModified("term.eval"))
		return true;

	return false;
}


bool TermMaxEvalOp::operate(StateP state)
{
	if(state->getEvaluations() >= nEvaluations_) {
		state->setTerminateCond();
		ECF_LOG(state, 1, "Termination: max evaluations reached (" + uint2str(nEvaluations_) + ")");
	}

	return true;
}