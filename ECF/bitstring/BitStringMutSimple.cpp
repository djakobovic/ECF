#include "../ECF_base.h"
#include "BitString.h"

namespace BitString
{

void BitStringMutSimple::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "mut.simple", (voidP) new double(0), ECF::DOUBLE);
	myGenotype_->registerParameter(state, "mut.simple.bitprob", (voidP) new double(0.001), ECF::DOUBLE);
}


bool BitStringMutSimple::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "mut.simple");
	probability_ = *((double*)sptr.get());

	sptr = myGenotype_->getParameterValue(state, "mut.simple.bitprob");
	bitProb_ = *((double*)sptr.get());

	bUseBitProb_ = false;
	if(myGenotype_->isParameterDefined(state, "mut.simple.bitprob"))
		bUseBitProb_ = true;

	return true;
}


bool BitStringMutSimple::mutate(GenotypeP gene)
{
	BitString* bitstr = (BitString*) (gene.get());

	// invert all bits with 'bitProb_' probability
	if(bUseBitProb_) {
		for(uint i = 0; i < bitstr->bits.size(); i++)
			if(state_->getRandomizer()->getRandomDouble() < bitProb_)
				bitstr->bits[i] = !bitstr->bits[i];
	}
	// invert a single random bit in the genotype
	else {
		uint iBit = state_->getRandomizer()->getRandomInteger(0, (int) bitstr->bits.size() - 1);
		bitstr->bits[iBit] = !(bitstr->bits[iBit]);
	}

	return true;
}

}