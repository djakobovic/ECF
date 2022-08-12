#include "../ECF_base.h"
#include "FloatingPoint.h"


namespace FloatingPoint
{

void FloatingPointCrsRandom::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.random", (voidP) new double(0), ECF::DOUBLE);
}


bool FloatingPointCrsRandom::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.random");
	probability_ = *((double*)sptr.get());
	return true;
}


bool FloatingPointCrsRandom::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	FloatingPoint* p1 = (FloatingPoint*) (gen1.get());
	FloatingPoint* p2 = (FloatingPoint*) (gen2.get());
	FloatingPoint* ch = (FloatingPoint*) (child.get());

	double a = state_->getRandomizer()->getRandomDouble();

	//dvije random jedinke jedan child je x neki crx prvi random, drugi child je y neki crx drugi random

	switch (state_->getRandomizer()->getRandomInteger(0, 1)) {
		case 0: for (uint i = 0; i < p1->realValue.size(); i++) {
					ch->realValue[i] = (p1->realValue[i] + (p1->getLBound()+ state_->getRandomizer()->getRandomDouble()*(p1->getUBound() - p1->getLBound())))* 0.5;
				}
				break;
		case 1: for (uint i = 0; i < p1->realValue.size(); i++) {
					ch->realValue[i] = (p2->realValue[i] + (p2->getLBound()+ state_->getRandomizer()->getRandomDouble()*(p2->getUBound() - p1->getLBound())))* 0.5;
				}
	}

	return true;
}

}