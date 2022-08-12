#include "../ECF_base.h"
#include "FloatingPoint.h"

namespace FloatingPoint
{

void FloatingPointCrsArithmetic::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.arithmetic", (voidP) new double(0), ECF::DOUBLE);
}


bool FloatingPointCrsArithmetic::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.arithmetic");
	probability_ = *((double*)sptr.get());
	return true;
}


bool FloatingPointCrsArithmetic::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	FloatingPoint* p1 = (FloatingPoint*) (gen1.get());
	FloatingPoint* p2 = (FloatingPoint*) (gen2.get());
	FloatingPoint* ch = (FloatingPoint*) (child.get());

	double a = state_->getRandomizer()->getRandomDouble();

	switch (state_->getRandomizer()->getRandomInteger(0, 1)) {
		case 0: for (uint i = 0; i < p1->realValue.size(); i++) {
					ch->realValue[i] = a * (p1->realValue[i]) + (1 - a) * (p2->realValue[i]);
				}
				break;
		case 1: for (uint i = 0; i < p1->realValue.size(); i++) {
					ch->realValue[i] = a * (p2->realValue[i]) + (1 - a) * (p1->realValue[i]);
				}
	}

	return true;
}

}