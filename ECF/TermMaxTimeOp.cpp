#include "ECF_base.h"
#include "TermMaxTimeOp.h"


void TermMaxTimeOp::registerParameters(StateP state)
{
	uint *value = new uint(0);
	state->getRegistry()->registerEntry("term.maxtime", (voidP) value, ECF::UINT,
		"max number of seconds to run (default: none)");
}


bool TermMaxTimeOp::initialize(StateP state)
{
	if(!state->getRegistry()->isModified("term.maxtime"))
		return false;

	voidP sptr = state->getRegistry()->getEntry("term.maxtime");
	maxTime_ = *((uint*) sptr.get());

	return true;
}


bool TermMaxTimeOp::operate(StateP state)
{
	if(state->getElapsedTime() > maxTime_) {
		state->setTerminateCond();
		ECF_LOG(state, 1, "Termination: designated time elapsed. (" + uint2str(maxTime_) + " seconds)");
	}

	return true;
}