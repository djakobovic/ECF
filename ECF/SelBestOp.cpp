#include "ECF_base.h"
#include "SelBestOp.h"

bool SelBestOp::initialize(StateP state)
{
	state_ = state;
	return true;
}


IndividualP SelBestOp::select(const std::vector<IndividualP>& pool)
{
	if(pool.empty())
		return IndividualP();

	IndividualP best = pool[0];
	for (uint i = 1; i < pool.size(); i++) {
		if (!(best->fitness->isBetterThan(pool[i]->fitness))){
			best = pool[i];
		}
	}

	return best;
}

