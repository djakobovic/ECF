#include "../ECF_base.h"
#include "Permutation.h"
#include <map>


namespace Permutation
{

void PermutationCrsOPX::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.OPX", (voidP) new double(0), ECF::DOUBLE);
}


bool PermutationCrsOPX::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.OPX");
	probability_ = *((double*)sptr.get());
	return true;
}


bool PermutationCrsOPX::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	Permutation* p1 = (Permutation*) (gen1.get());
	Permutation* p2 = (Permutation*) (gen2.get());
	Permutation* ch = (Permutation*) (child.get());

	int position = state_->getRandomizer()->getRandomInteger(p1->getSize()-1);

	// set representing values copied from first parent
	std::map<int, int> subSet;

	// copy first part of first parent (up to element at "position") into child
	for(int i = 0; i <= position; i++) {
		ch->variables[i] = p1->variables[i];
        // remember visited element
		subSet[p1->variables[i]] = 0;
	}

    // How many values are left?
	int left = (int) p1->getSize() - position - 1;
    // Continue filling child from next position:
	int indexChild = position + 1;

	// Go through each value of second parent in correct order;
	// if value is not already visited, copy it to child in first available position
	for(int ind2 = 0; ind2 < (int) p1->getSize() && left>0; ind2++) {
		if(subSet.find(p2->variables[ind2]) == subSet.end()) {
			ch->variables[indexChild] = p2->variables[ind2];
		    left--;
		    indexChild++;
		}
	}

	return true;
}

}
