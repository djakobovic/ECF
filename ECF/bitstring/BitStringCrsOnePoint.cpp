#include "../ECF_base.h"
#include "BitString.h"

namespace BitString
{

void BitStringCrsOnePoint::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.onepoint", (voidP) new double(0), ECF::DOUBLE);
}


bool BitStringCrsOnePoint::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.onepoint");
	probability_ = *((double*)sptr.get());
	return true;
}


bool BitStringCrsOnePoint::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	BitString* p1 = (BitString*) (gen1.get());
	BitString* p2 = (BitString*) (gen2.get());
	BitString* ch = (BitString*) (child.get());

	uint bitCrs = state_->getRandomizer()->getRandomInteger(0, (int) p1->bits.size() - 1);

	switch (state_->getRandomizer()->getRandomInteger(0, 1)) {
	case 0: for (uint i=0; i < bitCrs; i++) {
				ch->bits[i] = p1->bits.at(i);
			}
			for (uint i=bitCrs; i < p2->bits.size(); i++) {
				ch->bits[i] = p2->bits.at(i);
			}
			break;
	case 1: for (uint i=0; i < bitCrs; i++) {
				ch->bits[i] = p2->bits.at(i);
			}
			for (uint i=bitCrs; i < p1->bits.size(); i++) {
				ch->bits[i] = p1->bits.at(i);
			}
	}

	return true;
}

}