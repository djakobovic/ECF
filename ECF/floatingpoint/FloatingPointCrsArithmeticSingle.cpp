#include "../ECF_base.h"
#include "FloatingPoint.h"


namespace FloatingPoint
{

void FloatingPointCrsArithmeticSingle::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.arithmeticsingle", (voidP) new double(0), ECF::DOUBLE);
}


bool FloatingPointCrsArithmeticSingle::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.arithmeticsingle");
	probability_ = *((double*)sptr.get());
	return true;
}


bool FloatingPointCrsArithmeticSingle::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	FloatingPoint* p1 = (FloatingPoint*) (gen1.get());
	FloatingPoint* p2 = (FloatingPoint*) (gen2.get());
	FloatingPoint* ch = (FloatingPoint*) (child.get());

	double a = state_->getRandomizer()->getRandomDouble();
	uint dimensionCrs = state_->getRandomizer()->getRandomInteger((int) p1->realValue.size());

	switch (state_->getRandomizer()->getRandomInteger(0, 1)) {
		case 0: for (uint i = 0; i < dimensionCrs; i++) {
					ch->realValue[i] = p1->realValue[i];
				}
				ch->realValue[dimensionCrs] = a * (p2->realValue[dimensionCrs]) + (1 - a) * p1->realValue[dimensionCrs];
				for (uint i = dimensionCrs+1; i < p1->realValue.size(); i++) {
					ch->realValue[i] = p1->realValue[i];
				}
				break;
		case 1: for (uint i = 0; i < dimensionCrs; i++) {
					ch->realValue[i] = p2->realValue[i];
				}
				ch->realValue[dimensionCrs] = a * (p1->realValue[dimensionCrs]) + (1 - a) * p2->realValue[dimensionCrs];
				for (uint i = dimensionCrs+1; i < p2->realValue.size(); i++) {
					ch->realValue[i] = p2->realValue[i];
				}
	}

	return true;
}

}