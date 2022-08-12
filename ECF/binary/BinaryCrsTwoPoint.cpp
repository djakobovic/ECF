#include "../ECF_base.h"
#include "Binary.h"

namespace Binary
{

void BinaryCrsTwoPoint::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.twopoint", (voidP) new double(0), ECF::DOUBLE);
}


bool BinaryCrsTwoPoint::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.twopoint");
	probability_ = *((double*)sptr.get());
	return true;
}


bool BinaryCrsTwoPoint::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	Binary* p1 = (Binary*) (gen1.get());
	Binary* p2 = (Binary*) (gen2.get());
	Binary* ch = (Binary*) (child.get());

	uint bitCrsFirst = state_->getRandomizer()->getRandomInteger(p1->getNumBits());
	uint bitCrsSecond, tmp;
	do
	{
		bitCrsSecond = state_->getRandomizer()->getRandomInteger(p1->getNumBits());
	}while	(bitCrsFirst==bitCrsSecond);
	
	if (bitCrsFirst>bitCrsSecond)
	{
		tmp=bitCrsFirst;
		bitCrsFirst=bitCrsSecond;
		bitCrsSecond=tmp;
	}
	
	for (uint dimension = 0; dimension < p1->variables.size(); dimension++) {
		switch (state_->getRandomizer()->getRandomInteger(0, 1)) {
			case 0: for (uint i = 0; i < bitCrsFirst; i++) {
						ch->variables[dimension][i] = p1->variables[dimension][i];
					}
					for (uint i = bitCrsFirst; i < bitCrsSecond; i++) {
						ch->variables[dimension][i] = p2->variables[dimension][i];
					}
					for (uint i = bitCrsSecond; i < p1->getNumBits(); i++) {
						ch->variables[dimension][i] = p1->variables[dimension][i];
					}
					break;
			case 1: for (uint i = 0; i < bitCrsFirst; i++) {
						ch->variables[dimension][i] = p2->variables[dimension][i];
					}
					for (uint i = bitCrsFirst; i < bitCrsSecond; i++) {
						ch->variables[dimension][i] = p1->variables[dimension][i];
					}
					for (uint i = bitCrsSecond; i < p2->getNumBits(); i++) {
						ch->variables[dimension][i] = p2->variables[dimension][i];
					}
		}
	}

	// update integer and real domain representation
	ch->update();

	return true;
}

}