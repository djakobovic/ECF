#include "../ECF_base.h"
#include "Binary.h"

namespace Binary
{

void BinaryCrsHalfUniform::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.halfuniform", (voidP) new double(0), ECF::DOUBLE);
}


bool BinaryCrsHalfUniform::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.halfuniform");
	probability_ = *((double*)sptr.get());
	return true;
}

//swap half of differing bits
bool BinaryCrsHalfUniform::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
    Binary* p1 = (Binary*) (gen1.get());
	Binary* p2 = (Binary*) (gen2.get());
	Binary* ch = (Binary*) (child.get());

	uint counter = 0;
    for (uint dimension = 0; dimension < p1->variables.size(); dimension++) {
		for(uint i = 0; i < p1->getNumBits(); i++) {
			if (p1->variables[dimension][i] != p2->variables[dimension][i]) 
			counter++;
		}

		counter = (uint) counter/2;
		uint parent = state_->getRandomizer()->getRandomInteger(0, 1);

		for(uint i = 0; i < p1->getNumBits(); i++) {
			if (p1->variables[dimension][i] == p2->variables[dimension][i]) 
				ch->variables[dimension][i] = p1->variables[dimension][i];
			else {
				if (counter !=0) {
					ch->variables[dimension][i] = state_->getRandomizer()->getRandomInteger(2) ? true:false;
					counter--;
				}
				else {
					if (parent == 0) //take remaining bits from parent 1
						ch->variables[dimension][i] = p1->variables[dimension][i];
					else //take remaining bits from parent 2
						ch->variables[dimension][i] = p2->variables[dimension][i];
				}
			}
		}
		counter = 0;
	}

	ch->update();

	return true;
}

}