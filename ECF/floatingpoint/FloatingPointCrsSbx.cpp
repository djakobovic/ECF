#include "../ECF_base.h"
#include "FloatingPoint.h"
#include <math.h>


namespace FloatingPoint
{

void FloatingPointCrsSbx::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.sbx", (voidP) new double(0), ECF::DOUBLE);
	state->getRegistry()->registerEntry("crx.sbx.ni", (voidP) new uint(1), ECF::UINT);
}


bool FloatingPointCrsSbx::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.sbx");
	probability_ = *((double*)sptr.get());

	voidP par = state->getRegistry()->getEntry("crx.sbx.ni");
	ni = *((uint*) par.get());

	return true;
}


bool FloatingPointCrsSbx::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	FloatingPoint* p1 = (FloatingPoint*) (gen1.get());
	FloatingPoint* p2 = (FloatingPoint*) (gen2.get());
	FloatingPoint* ch = (FloatingPoint*) (child.get());

	// repeat for each variable
	for(uint i = 0; i < p1->realValue.size(); i++) {
		double p1x = p1->realValue[i];
		double p2x = p2->realValue[i];
		
		// determine smaller and greater parent values
		double low = p1x, high = p2x;
		if(p2x < p1x) {
			low  = p2x;
			high = p1x;
		}

		// check for close or same parents
		if(fabs(high - low) < 1.e-12) {
			ch->realValue[i] = (high + low) / 2;
			continue;
		}

		// determine min[(low - LBound), (UBound - high)]
		double min = low - p1->getLBound();
		if((p1->getUBound() - high) < min)
			min = p1->getUBound() - high;

		// determine beta and alpha
		double beta = 1 + 2 * min / (high - low);
		double alpha = 2 - 1 / pow(beta, 1. + ni);

		double u = state_->getRandomizer()->getRandomDouble();
		// scale down to avoid u == 1
		u *= 0.999;

		double beta_dash;
		// if u is smaller than 1/alpha perform a contracting crossover
		if (u <= (1. / alpha)) {
			beta_dash = pow(alpha * u, 1.0 / (ni + 1.0));
		}
		// otherwise perform an expanding crossover
		else {
			beta_dash = pow(1. / (2. - alpha * u), 1.0 / (ni + 1.0));
		}

		// apply beta_dash
		switch (state_->getRandomizer()->getRandomInteger(0, 1)) {
			case 0: ch->realValue[i] = ((p1->realValue[i] + p2->realValue[i])/2.0) - beta_dash * 0.5 * fabs(p1->realValue[i] - p2->realValue[i]);
					break;
			case 1: ch->realValue[i] = ((p1->realValue[i] + p2->realValue[i])/2.0) + beta_dash * 0.5 * fabs(p1->realValue[i] - p2->realValue[i]);
		}
	}

	return true;
}

}
