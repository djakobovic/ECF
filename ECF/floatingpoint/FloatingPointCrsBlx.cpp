#include "../ECF_base.h"
#include "FloatingPoint.h"


namespace FloatingPoint
{

void FloatingPointCrsBlx::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.blx", (voidP) new double(0), ECF::DOUBLE);
	myGenotype_->registerParameter(state, "crx.blx.alpha", (voidP) new double(0.5), ECF::DOUBLE);
}


bool FloatingPointCrsBlx::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.blx");
	probability_ = *((double*)sptr.get());

	useAlpha = false;
	if(myGenotype_->isParameterDefined(state, "crx.blx.alpha"))
		useAlpha = true;

	sptr = myGenotype_->getParameterValue(state, "crx.blx.alpha");
	alpha = *((double*)sptr.get());

	return true;
}


bool FloatingPointCrsBlx::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	FloatingPoint* p1 = (FloatingPoint*) (gen1.get());
	FloatingPoint* p2 = (FloatingPoint*) (gen2.get());
	FloatingPoint* ch = (FloatingPoint*) (child.get());

	double min1, max1, min2, max2, min, max, a, b, I;

	min1 = p1->realValue[0];
	max1 = p1->realValue[0];
	min2 = p2->realValue[0];
	max2 = p2->realValue[0];
	for (uint i = 1; i < p1->realValue.size(); i++) {
		if (p1->realValue[i] < min1)
			min1 = p1->realValue[i];
		if (p1->realValue[i] > max1)
			max1 = p1->realValue[i];

		if (p2->realValue[i] < min2)
			min2 = p2->realValue[i];
		if (p2->realValue[i] > max2)
			max2 = p2->realValue[i];
	}
	min = (min1 < min2)?min1:min2;
	max = (max1 > max2)?max1:max2;
	if(useAlpha)
		b = alpha;
	else
		b = state_->getRandomizer()->getRandomDouble();
	I = max - min;
	min = min - I*b;
	max = max + I*b;
	for (uint i = 0; i < p1->realValue.size(); i++) {
		a = state_->getRandomizer()->getRandomDouble();
		a = min + a*(max-min); // s obzirom da nema tako neceg u SimpleRandomizeru dodao sam ovdje
		ch->realValue[i] = a;
	}

	return true;
}

}