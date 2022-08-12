#include "../ECF_base.h"
#include "Binary.h"

namespace Binary
{

void BinaryCrsOnePoint::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.onepoint", (voidP) new double(0), ECF::DOUBLE);
}


bool BinaryCrsOnePoint::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.onepoint");
	probability_ = *((double*)sptr.get());
	return true;
}


bool BinaryCrsOnePoint::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	Binary* p1 = (Binary*) (gen1.get());
	Binary* p2 = (Binary*) (gen2.get());
	Binary* ch = (Binary*) (child.get());

	uint bitCrs = state_->getRandomizer()->getRandomInteger(p1->getNumBits());

	for (uint dimension = 0; dimension < p1->variables.size(); dimension++) {
		switch (state_->getRandomizer()->getRandomInteger(0, 1)) {
			case 0: for (uint i = 0; i < bitCrs; i++) {
						ch->variables[dimension][i] = p1->variables[dimension][i];
					}
					for (uint i = bitCrs; i < p2->getNumBits(); i++) {
						ch->variables[dimension][i] = p2->variables[dimension][i];
					}
					break;
			case 1: for (uint i = 0; i < bitCrs; i++) {
						ch->variables[dimension][i] = p2->variables[dimension][i];
					}
					for (uint i = bitCrs; i < p1->getNumBits(); i++) {
						ch->variables[dimension][i] = p1->variables[dimension][i];
					}
		}
	}

	// update integer and real domain representation
	ch->update();

	return true;
}

}