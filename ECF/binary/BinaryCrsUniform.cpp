#include "../ECF_base.h"
#include "Binary.h"

namespace Binary
{

void BinaryCrsUniform::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.uniform", (voidP) new double(0), ECF::DOUBLE);
}


bool BinaryCrsUniform::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.uniform");
	probability_ = *((double*)sptr.get());
	return true;
}


bool BinaryCrsUniform::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
    Binary* p1 = (Binary*) (gen1.get());
	Binary* p2 = (Binary*) (gen2.get());
	Binary* ch = (Binary*) (child.get());

    for (uint dimension = 0; dimension < p1->variables.size(); dimension++) {
		for(uint i = 0; i < p1->getNumBits(); i++) {
			if (p1->variables[dimension][i] == p2->variables[dimension][i]) 
				ch->variables[dimension][i] = p1->variables[dimension][i];
			else
				ch->variables[dimension][i] = state_->getRandomizer()->getRandomInteger(2) ? true:false;
		}
	}

	ch->update();

	return true;
}

}