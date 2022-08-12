#include "../ECF_base.h"
#include "Permutation.h"
#include <map>


namespace Permutation
{

void PermutationCrsOX::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.OX", (voidP) new double(0), ECF::DOUBLE);
}


bool PermutationCrsOX::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.OX");
	probability_ = *((double*)sptr.get());
	return true;
}


bool PermutationCrsOX::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	Permutation* p1 = (Permutation*) (gen1.get());
	Permutation* p2 = (Permutation*) (gen2.get());
	Permutation* ch = (Permutation*) (child.get());

	int start = state_->getRandomizer()->getRandomInteger(p1->getSize());
	int end = state_->getRandomizer()->getRandomInteger(start, (int) p1->getSize() - 1);

	std::map<int, int> subSet;
	
	int ind1 = state_->getRandomizer()->getRandomInteger(p1->getSize() - 1);
	int ind2 = state_->getRandomizer()->getRandomInteger(ind1, (int) p1->getSize() - 1);

	// copy subvector into child
	for(int i = ind1; i <= ind2; i++) {
		ch->variables[i] = p1->variables[i];
		subSet[p1->variables[i]] = 0;
	}
	int indexChild = (ind2 + 1) % p1->getSize();
	std::map<int, int>::iterator iter = subSet.begin();

	// krecemo od indeksa za jedan vise od indeksa zadnjeg elementa podniza
	// i kopiramo iz drugog roditelja u dijete
	// ako naidjemo na element koji se vec nalazi u podnizu idemo dalje 
	// postupak se zaustavlja kada popunimo cijelo dijete
	do {
		ind2 = (++ind2) % p1->getSize();
		if(subSet.find(p2->variables[ind2]) == subSet.end()) {
			ch->variables[indexChild] = p2->variables[ind2];
			indexChild = (++indexChild) % p1->getSize();
		}
	} while(indexChild != ind1);

	return true;
}

}