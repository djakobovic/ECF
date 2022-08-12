#include "../ECF_base.h"
#include "FloatingPoint.h"
#include<cmath>


namespace FloatingPoint
{

void FloatingPointCrsBlxAlphaBeta::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.blxalphabeta", (voidP) new double(0), ECF::DOUBLE);
	state->getRegistry()->registerEntry("alpha", (voidP) new double(0.75), ECF::DOUBLE);
	state->getRegistry()->registerEntry("beta", (voidP) new double(0.25), ECF::DOUBLE);
}


bool FloatingPointCrsBlxAlphaBeta::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.blxalphabeta");
	probability_ = *((double*)sptr.get());

	voidP par = state->getRegistry()->getEntry("alpha");
	alpha = *((double*) par.get());

	par = state->getRegistry()->getEntry("beta");
	beta = *((double*) par.get());

	return true;
}


bool FloatingPointCrsBlxAlphaBeta::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	FloatingPoint* p1 = (FloatingPoint*) (gen1.get());
	FloatingPoint* p2 = (FloatingPoint*) (gen2.get());
	FloatingPoint* ch = (FloatingPoint*) (child.get());

	double min = 0, max = 0, a, I;

	for (uint i = 1; i < p1->realValue.size(); i++) {
		a = state_->getRandomizer()->getRandomDouble();

		I = fabs(p1->realValue[i] - p2->realValue[i]);

		if (p1->realValue[i] <= p2->realValue[i]){
			min = p1->realValue[i] - I*alpha;
			max = p2->realValue[i] + I*beta;
		}
		else{
			min = p2->realValue[i] - I*beta;
			max = p1->realValue[i] + I*alpha;
		}
		ch->realValue[i] = min + a * (max - min);
	}

	return true;
}

}