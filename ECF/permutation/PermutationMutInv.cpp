#include "../ECF_base.h"
#include "Permutation.h"


namespace Permutation
{

void PermutationMutInv::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "mut.inv", (voidP) new double(0), ECF::DOUBLE);
}


bool PermutationMutInv::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "mut.inv");
	probability_ = *((double*)sptr.get());
	return true;
}


bool PermutationMutInv::mutate(GenotypeP gene)
{
	Permutation* perm = (Permutation*) (gene.get());

	//najviše moemo uzeti predzandnji element kao poèetak podniza
	int ind1 = state_->getRandomizer()->getRandomInteger(perm->getSize() - 1);
	int ind2 = state_->getRandomizer()->getRandomInteger(ind1, (int) perm->getSize() - 1);
	int temp = perm->variables[ind2];
	int distance = ind2 -ind1 + 1;

	for(int i = 0; i < distance / 2; i++) {
		temp = perm->variables[ind1 + i];
		perm->variables[ind1 + i] = perm->variables[ind2 - i];
		perm->variables[ind2 - i] = temp;
	}

	return true;
}

}