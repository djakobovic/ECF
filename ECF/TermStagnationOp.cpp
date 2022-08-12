#include "ECF_base.h"
#include "TermStagnationOp.h"
#include <cmath>


void TermStagnationOp::registerParameters(StateP state)
{
	uint *value = new uint(50);
	state->getRegistry()->registerEntry("term.stagnation", (voidP) value, ECF::UINT,
		"max number of consecutive generations without improvement (default: 5000 / pop_size)");
}


bool TermStagnationOp::initialize(StateP state)
{
	voidP sptr = state->getRegistry()->getEntry("term.stagnation");
	termStagnation_ = *((uint*) sptr.get());

	// define the default criterion
	if(termStagnation_ == 0) {
		voidP sptr = state->getRegistry()->getEntry("population.size");
		uint demeSize = *((uint*) sptr.get());
		termStagnation_ = 5000 / demeSize;
		if(termStagnation_ < 10)
			termStagnation_ = 5;
		if(termStagnation_ > 200)
			termStagnation_ = 200;
	}

	if(!state->getRegistry()->isModified("term.stagnation"))
		return false;

	return true;
}


bool TermStagnationOp::operate(StateP state)
{
	uint currentGen = state->getGenerationNo();
	if(currentGen - state->getPopulation()->getHof()->getLastChange() > termStagnation_) {
		state->setTerminateCond();
		ECF_LOG(state, 1, "Termination: maximum number of generations without improvement ("
			+ uint2str(termStagnation_) + ") reached");
	}

	return true;
}