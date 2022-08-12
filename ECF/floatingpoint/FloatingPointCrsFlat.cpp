#include "../ECF_base.h"
#include "FloatingPoint.h"
#include<cmath>


namespace FloatingPoint
{

void FloatingPointCrsFlat::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.flat", (voidP) new double(0), ECF::DOUBLE);
}


bool FloatingPointCrsFlat::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.flat");
	probability_ = *((double*)sptr.get());

	return true;
}


bool FloatingPointCrsFlat::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	FloatingPoint* p1 = (FloatingPoint*) (gen1.get());
	FloatingPoint* p2 = (FloatingPoint*) (gen2.get());
	FloatingPoint* ch = (FloatingPoint*) (child.get());

	double min, max, a, I;

	for (uint i = 1; i < p1->realValue.size(); i++) {
		a = state_->getRandomizer()->getRandomDouble();
		if (p1->realValue[i] < p2->realValue[i]){
			min = p1->realValue[i];
		}
		else {
			min = p2->realValue[i];
		}
		if (p1->realValue[i] > p2->realValue[i]){
			max = p1->realValue[i];
		}
		else {
			max = p2->realValue[i];
		}
		I = fabs(max - min);
		ch->realValue[i] = min + a * fabs(max - min);
	}

	return true;
}

}