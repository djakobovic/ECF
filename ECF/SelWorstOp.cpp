#include "ECF_base.h"
#include "SelWorstOp.h"


bool SelWorstOp::initialize(StateP state)
{
	state_ = state;
	return true;
}


IndividualP SelWorstOp::select(const std::vector<IndividualP>& pool)
{
	if(pool.empty())
		return IndividualP();

	IndividualP worst = pool[0];
	for (uint i = 1; i<pool.size(); i++) {
		if (worst->fitness->isBetterThan(pool[i]->fitness)){
			worst = pool[i];
		}
	}
		
	return worst;
}

