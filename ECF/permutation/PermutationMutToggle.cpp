#include "../ECF_base.h"
#include "Permutation.h"


namespace Permutation
{

void PermutationMutToggle::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "mut.toggle", (voidP) new double(0), ECF::DOUBLE);
}


bool PermutationMutToggle::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "mut.toggle");
	probability_ = *((double*)sptr.get());
	return true;
}


bool PermutationMutToggle::mutate(GenotypeP gene)
{
	Permutation* perm = (Permutation*) (gene.get());

	int ind1 = state_->getRandomizer()->getRandomInteger(perm->getSize());
	int ind2 = state_->getRandomizer()->getRandomInteger(perm->getSize());
	int temp = perm->variables[ind1];
	perm->variables[ind1] = perm->variables[ind2];
	perm->variables[ind2] = temp;

	return true;
}

}