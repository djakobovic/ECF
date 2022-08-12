#include "../ECF_base.h"
#include "Binary.h"

namespace Binary
{

void BinaryCrsMasked::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.masked", (voidP) new double(0), ECF::DOUBLE);
}


bool BinaryCrsMasked::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.masked");
	probability_ = *((double*)sptr.get());
	return true;
}


bool BinaryCrsMasked::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	Binary* p1 = (Binary*) (gen1.get());
	Binary* p2 = (Binary*) (gen2.get());
	Binary* ch = (Binary*) (child.get());

	uint rand;

	rand = state_->getRandomizer()->getRandomInteger(0, 1);

	for (uint dimension = 0; dimension < p1->variables.size(); dimension++) {
		uint i;
		switch (rand) {
			case 0: for (i = 0; i < p1->getNumBits(); i++) {
						ch->variables[dimension][i] = p1->variables[dimension][i];
					}
					break;
			case 1: for (i = 0; i < p2->getNumBits(); i++) {
						ch->variables[dimension][i] = p2->variables[dimension][i];
					}
		}
	}

	for (uint dimension = 0; dimension < p1->variables.size(); dimension++) {
		uint i;
		switch (rand) {
			case 0: for (i = 0; i < p1->getNumBits(); i++) {
						if (p2->variables[dimension][i] == 1 && p1->variables[dimension][i] == 0)
							ch->variables[dimension][i] = p2->variables[dimension][i];
					}
					break;
			case 1: for (i = 0; i < p2->getNumBits(); i++) {
						if (p2->variables[dimension][i] == 0 && p1->variables[dimension][i] == 1)
							ch->variables[dimension][i] = p1->variables[dimension][i];
					}
		}
	}

	// update integer and real domain representation
	ch->update();

	return true;
}

}