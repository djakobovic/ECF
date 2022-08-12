#include "../ECF_base.h"
#include "Permutation.h"


namespace Permutation
{

void PermutationMutIns::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "mut.ins", (voidP) new double(0), ECF::DOUBLE);
}


bool PermutationMutIns::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "mut.ins");
	probability_ = *((double*)sptr.get());
	return true;
}


bool PermutationMutIns::mutate(GenotypeP gene)
{
	Permutation* perm = (Permutation*) (gene.get());

	//navjeæi indeks za prvi element insert mutacije
	//mora biti maksimalno predzadnji u nizu
	int ind1 = state_->getRandomizer()->getRandomInteger(perm->getSize() - 1);
	int ind2 = state_->getRandomizer()->getRandomInteger(ind1, (int) perm->getSize() - 1);
	int temp = perm->variables[ind2];

	//posmicemo udesno
	for(int i = ind2; i > ind1; i--) {
		//temp=perm->variables[i];
		perm->variables[i] = perm->variables[i - 1];
	}
	//za slucaj da su indeksi razliciti stavljamo element koji smo prebrisali
	//tj zadnji element podniza iza prvog elementa
	//inace se for petlja uopce nije izvrsila pa bismo 2x isti element imali
	if(ind1 != ind2)
		perm->variables[ind1 + 1] = temp;

	return true;
}

}