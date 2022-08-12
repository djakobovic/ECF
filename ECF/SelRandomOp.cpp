#include "ECF_base.h"
#include "SelRandomOp.h"


bool SelRandomOp::initialize(StateP state)
{
	state_ = state;
    randomizer_ = state->getRandomizer();
    return true;
}


IndividualP SelRandomOp::select(const std::vector<IndividualP>& pool)
{
	if(pool.empty())
		return IndividualP();

	return pool[ randomizer_->getRandomInteger(0, (int) pool.size()-1) ];
}

