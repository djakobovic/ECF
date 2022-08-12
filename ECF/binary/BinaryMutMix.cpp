#include "../ECF_base.h"
#include "Binary.h"

namespace Binary
{

void BinaryMutMix::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "mut.mix", (voidP) new double(0), ECF::DOUBLE);
}


bool BinaryMutMix::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "mut.mix");
	probability_ = *((double*)sptr.get());
	return true;
}


bool BinaryMutMix::mutate(GenotypeP gene)
{
	Binary* bin = (Binary*) (gene.get());

	uint dimension = state_->getRandomizer()->getRandomInteger((uint) bin->variables.size());
	
	// make sure indexSmaller < indexBigger
	uint bitIndexSmaller = state_->getRandomizer()->getRandomInteger((uint) bin->getNumBits());
	uint bitIndexBigger;
	do {
		bitIndexBigger = state_->getRandomizer()->getRandomInteger((uint) bin->getNumBits());
	}while(bitIndexBigger == bitIndexSmaller);
	
	uint tmp = bitIndexSmaller;
	if(bitIndexSmaller > bitIndexBigger) {
	  bitIndexSmaller = bitIndexBigger;
	  bitIndexBigger = tmp;
	}

	// count 1's and 0's
	int counter0 = 0;
	int counter1 = 0;
	
	for(uint i = bitIndexSmaller; i <= bitIndexBigger; i++) {
		if(bin->variables[dimension][i]) counter1++;
		else counter0++;
	}
	
	int fairness0 = counter0;
	int fairness1 = counter1;
	
	// mix bits in [indexSmaller, indexBigger]
	for(uint i = bitIndexSmaller; i <= bitIndexBigger; i++) {
		int random = state_->getRandomizer()->getRandomInteger(1, fairness0 + fairness1);
		if(random <= fairness1) {
			if(counter1 > 0) {
				bin->variables[dimension][i] = true;
				counter1--;
			}
			else {
				bin->variables[dimension][i] = false;
				counter0--;
			}
		}
		else {
			if(counter0 > 0) {
				bin->variables[dimension][i] = false;
				counter0--;
			}
			else {
				bin->variables[dimension][i] = true;
				counter1--;
			}
		}
	}
	
	bin->update();

	return true;
}

}