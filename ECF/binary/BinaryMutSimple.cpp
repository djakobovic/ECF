#include "../ECF_base.h"
#include "Binary.h"
#include <cmath>

namespace Binary
{

void BinaryMutSimple::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "mut.simple", (voidP) new double(0), ECF::DOUBLE);
	myGenotype_->registerParameter(state, "mut.simple.bitprob", (voidP) new double(0.001), ECF::DOUBLE);
}


bool BinaryMutSimple::initialize(StateP state)
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


bool BinaryMutSimple::mutate(GenotypeP gene)
{
	Binary* bin = (Binary*) (gene.get());

	// invert all bits with 'bitProb_' probability
	if(bUseBitProb_) {
		for(uint i = 0; i < bin->variables.size(); i++)
			for(uint j = 0; j < bin->getNumBits(); j++)
				if(state_->getRandomizer()->getRandomDouble() < bitProb_)
					bin->variables[i][j] = !(bin->variables[i][j]);
	}
	// invert a single random bit in the genotype
	else {
		uint iBit = state_->getRandomizer()->getRandomInteger((uint) bin->getNumBits());
		uint dimension = state_->getRandomizer()->getRandomInteger((uint) bin->variables.size());
		bin->variables[dimension][iBit] = !(bin->variables[dimension][iBit]);
	}

	bin->update();

/*

	//
	// izvedba jednostavnog lokalnog operatora
	//
	IndividualP myInd = state_->getAlgorithm()->mutation_->currentInd;
	FitnessP current, modified;
	double precision = 1 / pow(10., (int) bin->nDecimal_);

	current = state_->getAlgorithm()->evalOp_->evaluate(myInd);

	for(uint i = 0; i < bin->variables.size(); i++) {
		double newVal = bin->realValue[i];

		bin->realValue[i] += precision;
		modified = state_->getAlgorithm()->evalOp_->evaluate(myInd);
		if(modified->isBetterThan(current)) {
			current = modified;
			newVal = bin->realValue[i];
		}

		bin->realValue[i] -= 2 * precision;
		modified = state_->getAlgorithm()->evalOp_->evaluate(myInd);
		if(modified->isBetterThan(current)) {
			current = modified;
			newVal = bin->realValue[i];
		}

		bin->realValue[i] = newVal;
	}

	// update genotype according to realValue
	for(uint iVar = 0; iVar < bin->nDimension_; iVar++) {
		bin->decValue[iVar] = static_cast<long int> ((bin->realValue[iVar] - bin->minValue_) / (bin->maxValue_ - bin->minValue_) * bin->potention_);

		long dec = bin->decValue[iVar];
		for (int iBit = bin->nBits_; iBit > 0; dec = dec/2, iBit--) {
			bin->vBool_[iBit - 1] = (dec % 2) ? true:false;
		}
		bin->variables[iVar] = bin->vBool_;

		if(bin->bRounding_) {
			bin->realValue[iVar] = bin->round(bin->realValue[iVar], bin->nDecimal_);
		}
	}

*/
	return true;
}

}