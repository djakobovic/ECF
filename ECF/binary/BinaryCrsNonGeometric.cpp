#include "../ECF_base.h"
#include "Binary.h"

namespace Binary
{

void BinaryCrsNonGeometric::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.nongeometric", (voidP) new double(0), ECF::DOUBLE);
}


bool BinaryCrsNonGeometric::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.nongeometric");
	probability_ = *((double*)sptr.get());

	return true;
}


/*
Step 1: Let zi := xi for i =1,2, ...,n (i.e., z is a copy of the
		primary parent x).
Step 2: Let zi := (1 - zi) with a probability PBF when xi = yi
		where PBF is a prespecified bit-flip probability.
Note:	Random primary parent selection
*/
bool BinaryCrsNonGeometric::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	Binary* p1 = (Binary*) (gen1.get());
	Binary* p2 = (Binary*) (gen2.get());
	Binary* ch = (Binary*) (child.get());
	
	double PBF = 0.5; //prespecified bit-flip probability

	//Step 1
	for (uint dimension = 0; dimension < p1->variables.size(); dimension++) {
		switch (state_->getRandomizer()->getRandomInteger(0, 1)) {
			case 0: for (uint i = 0; i < p1->getNumBits(); i++) {
						ch->variables[dimension][i] = p1->variables[dimension][i];
					}
					break;
			case 1: for (uint i = 0; i < p2->getNumBits(); i++) {
						ch->variables[dimension][i] = p2->variables[dimension][i];
					}
		}
		//Step 2
		for(uint i = 0; i < p1->getNumBits(); i++) {
			if (p1->variables[dimension][i] == p2->variables[dimension][i]) {
				double changeProbability = state_->getRandomizer()->getRandomDouble();
				if (changeProbability > PBF)
					ch->variables[dimension][i] =  ch->variables[dimension][i]  ? false:true;				
			}
		}
	}

	// update integer and real domain representation
	ch->update();

	return true;
}

}