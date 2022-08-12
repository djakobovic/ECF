#include "../ECF_base.h"
#include "FloatingPoint.h"
#include<cmath>


namespace FloatingPoint
{

void FloatingPointCrsBlxAlpha::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.blxalpha", (voidP) new double(0), ECF::DOUBLE);
	state->getRegistry()->registerEntry("alpha", (voidP) new double(0.5), ECF::DOUBLE);
}


bool FloatingPointCrsBlxAlpha::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.blxalpha");
	probability_ = *((double*)sptr.get());

	voidP par = state->getRegistry()->getEntry("alpha");
	alpha = *((double*) par.get());
	return true;
}


bool FloatingPointCrsBlxAlpha::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	FloatingPoint* p1 = (FloatingPoint*) (gen1.get());
	FloatingPoint* p2 = (FloatingPoint*) (gen2.get());
	FloatingPoint* ch = (FloatingPoint*) (child.get());

	double min, max, a, I, resMin = 0, resMax = 0;

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
		resMin = min - I * alpha;
		resMax = max + I * alpha;
		ch->realValue[i] = resMin + a * fabs(resMax - resMin);
	}

	return true;
}

}