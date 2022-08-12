#include "../ECF_base.h"
#include "BitString.h"

namespace BitString
{

void BitStringMutMix::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "mut.mix", (voidP) new double(0), ECF::DOUBLE);
}


bool BitStringMutMix::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "mut.mix");
	probability_ = *((double*)sptr.get());

	sptr = myGenotype_->getParameterValue(state, "size");
	if(*((uint*)sptr.get()) == 1) {
		ECF_LOG_ERROR(state, "Warning: mixing mutation not applicable on BitString genotype of size 1!");
	}
	return true;
}


bool BitStringMutMix::mutate(GenotypeP gene)
{
	BitString* bitstr = (BitString*) (gene.get());

	if(bitstr->bits.size() == 1)
		return false;

	// determine random left and right bit bound
	uint bitIndexSmaller = state_->getRandomizer()->getRandomInteger((uint) bitstr->bits.size());
	uint bitIndexBigger;

	do {
		bitIndexBigger = state_->getRandomizer()->getRandomInteger((uint) bitstr->bits.size());
	} while (bitIndexSmaller == bitIndexBigger);

	// make sure smaller < bigger
	uint tmp = bitIndexSmaller;
	if (bitIndexSmaller > bitIndexBigger) {
      bitIndexSmaller = bitIndexBigger;
	  bitIndexBigger = tmp;
	}

	// count zeros and ones in chosen segment
	int counter0 = 0;
	int counter1 = 0;

    for(uint i = bitIndexSmaller; i <= bitIndexBigger; i++) {
	  if(bitstr->bits[i]) counter1++;
	  else counter0++;
	}

	int fairness0 = counter0;
	int fairness1 = counter1;

	// mutate chosen segment
    for(uint i = bitIndexSmaller; i <= bitIndexBigger; i++) {
	  int random = state_->getRandomizer()->getRandomInteger(1, fairness0 + fairness1);

	  if(random <= fairness1) {
		  if (counter1 > 0) {
			  bitstr->bits[i] = true;
			  counter1--;
		  }
		  else {
			  bitstr->bits[i] = false;
			  counter0--;
		  }
	  }
	  else {
		  if (counter0 > 0) {
			  bitstr->bits[i] = false;
			  counter0--;
		  }
		  else {
			  bitstr->bits[i] = true;
			  counter1--;
		  }
	  }
	}

	return true;
}

}