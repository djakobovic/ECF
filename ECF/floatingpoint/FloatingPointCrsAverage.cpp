#include "../ECF_base.h"
#include "FloatingPoint.h"

namespace FloatingPoint
{

void FloatingPointCrsAverage::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.average", (voidP) new double(0), ECF::DOUBLE);
}


bool FloatingPointCrsAverage::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.average");
	probability_ = *((double*)sptr.get());
	return true;
}


bool FloatingPointCrsAverage::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	FloatingPoint* p1 = (FloatingPoint*) (gen1.get());
	FloatingPoint* p2 = (FloatingPoint*) (gen2.get());
	FloatingPoint* ch = (FloatingPoint*) (child.get());

	for (uint i = 0; i < p1->realValue.size(); i++) {
		ch->realValue[i] = 0.5 * (p1->realValue[i]) + 0.5 * (p2->realValue[i]);
	}

	return true;
}

}