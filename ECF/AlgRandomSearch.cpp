#include "ECF_base.h"
#include "AlgRandomSearch.h"
#include "ECF_macro.h"


RandomSearch :: RandomSearch()
{
	name_ = "RandomSearch";
}


void RandomSearch :: registerParameters(StateP state)
{
}


bool RandomSearch :: initialize(StateP state)
{
	return true;
}


bool RandomSearch :: advanceGeneration(StateP state, DemeP deme)
{
	// initialize and evaluate all individuals
	for(uint i = 0; i < deme->size(); i++) {
		deme->at(i)->initialize(state);
		evaluate(deme->at(i));
	}

	return true;
}
