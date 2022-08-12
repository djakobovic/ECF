#include "../ECF_base.h"
#include "FloatingPoint.h"
#include <math.h>


namespace FloatingPoint
{

void FloatingPointCrsBga::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.bga", (voidP) new double(0), ECF::DOUBLE);
}


bool FloatingPointCrsBga::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.bga");
	probability_ = *((double*)sptr.get());

	return true;
}


bool FloatingPointCrsBga::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	FloatingPoint* p1 = (FloatingPoint*) (gen1.get());
	FloatingPoint* p2 = (FloatingPoint*) (gen2.get());
	FloatingPoint* ch = (FloatingPoint*) (child.get());

	int a;
	uint size = (uint) p1->realValue.size();
	// upper - lower bound
	double range = 0.5 * (p1->getUBound() - p1->getLBound());
	double gama = 0, lambda = 0, b;

	for (int i = 0; i <= 15; i++) {
		a = state_->getRandomizer()->getRandomInteger(1, 16);
		// 1/16 probability for 1 = 1
		if (a == 16)
			a = 1;
		else
			a = 0;
		gama = gama + a * pow((double) 2., -i);
	}

	double norm = 0;
	for(uint i = 0; i < size; i++)
		norm += pow(p1->realValue[i] - p2->realValue[i], 2);
	norm = sqrt(norm);

	// scaling safeguard 
	if(norm < 10e-9)
		norm = 1;

	// determine better parent
	FloatingPoint *better, *worse;
	FitnessP parent2 = state_->getContext()->secondParent->fitness;
	if(state_->getContext()->firstParent->fitness->isBetterThan(parent2)) {
		better = p1;
		worse = p2;
	} else {
		better = p2;
		worse = p1;
	}

	// build child
	for (uint i = 0; i < size; i++) {
		// worse gene minus better gene divided with norm
		lambda = (worse->realValue[i] - better->realValue[i]) / norm;

		b = state_->getRandomizer()->getRandomDouble();
		// minus with probability 0.9
		if (b <= 0.9)
			ch->realValue[i] = better->realValue[i] - range * gama * lambda;
		else
			ch->realValue[i] = better->realValue[i] + range * gama * lambda;

		// check for bounds; if outside the interval, bring closer to better parent
		if(ch->realValue[i] > ch->getUBound())
			ch->realValue[i] = better->realValue[i] + state_->getRandomizer()->getRandomDouble() * (ch->getUBound() - better->realValue[i]);
		else if(ch->realValue[i] < ch->getLBound())
			ch->realValue[i] = better->realValue[i] - state_->getRandomizer()->getRandomDouble() * (better->realValue[i] - ch->getLBound());
	}

	return true;
}

}
