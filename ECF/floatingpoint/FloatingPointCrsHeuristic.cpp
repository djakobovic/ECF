#include "../ECF_base.h"
#include "FloatingPoint.h"


namespace FloatingPoint
{

void FloatingPointCrsHeuristic::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.heuristic", (voidP) new double(0), ECF::DOUBLE);
}


bool FloatingPointCrsHeuristic::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.heuristic");
	probability_ = *((double*)sptr.get());
	return true;
}


bool FloatingPointCrsHeuristic::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	FloatingPoint* p1 = (FloatingPoint*) (gen1.get());
	FloatingPoint* p2 = (FloatingPoint*) (gen2.get());
	FloatingPoint* ch = (FloatingPoint*) (child.get());

	double a;

	for (uint i = 1; i < p1->realValue.size(); i++) {
		a = state_->getRandomizer()->getRandomDouble();
		if (p1->realValue[i] <= p2->realValue[i])
			ch->realValue[i] = p1->realValue[i] + a*(p2->realValue[i] - p1->realValue[i]);
		else
			ch->realValue[i] = p2->realValue[i] + a*(p1->realValue[i] - p2->realValue[i]);
	}

	return true;
}

}