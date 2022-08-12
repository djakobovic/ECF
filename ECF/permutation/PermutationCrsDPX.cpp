#include "../ECF_base.h"
#include "Permutation.h"
#include <map>


namespace Permutation
{

void PermutationCrsDPX::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.DPX", (voidP) new double(0), ECF::DOUBLE);
}


bool PermutationCrsDPX::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.DPX");
	probability_ = *((double*)sptr.get());
	return true;
}


bool PermutationCrsDPX::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	Permutation* p1 = (Permutation*) (gen1.get());
	Permutation* p2 = (Permutation*) (gen2.get());
	Permutation* ch = (Permutation*) (child.get());

    int capacity = (int) p1->getSize();

    // Flag positions in child on which values are set from both parents
    bool* takenPositions = new bool[capacity];

    // A collection of values which are not copied from first parent;
    // it is growable collection of maximally "capacity" elements;
    // current size is given by "unusedNo" in which case elements occupy
    // positions from 0 to unusedNo-1
    int unusedNo = 0;
    int* unusedElements = new int[capacity];

    // Mark all positions as unused
    for(int i = 0; i < capacity; i++) {
        takenPositions[i] = false;
    }

    // Go through parents; if values at same index are the same,
    // copy value to child at that position and mark position as taken;
    // otherwise, add element from first parent to unused elements
    for(int ind = 0; ind < capacity; ind++) {
        if(p1->variables[ind]==p2->variables[ind]) {
            ch->variables[ind] = p1->variables[ind];
            takenPositions[ind] = true;
        } else {
            unusedElements[unusedNo] = p1->variables[ind];
            unusedNo++;
        }
    }

    // Candidate index for next empty position in child:
    int candidatePosition = 0;

    // While having at least two unused elements left, pick one randomly
    // and add it to child; remove it from unused collection and shrink it.
    while(unusedNo > 1) {
        int index = state_->getRandomizer()->getRandomInteger(unusedNo);
        while(takenPositions[candidatePosition]) {
            candidatePosition++;
        }
        ch->variables[candidatePosition] = unusedElements[index];
        unusedNo--;
        unusedElements[index] = unusedElements[unusedNo];
        candidatePosition++;
    }
    // If there is remaining unused element, copy it to child:
    if(unusedNo!=0) {
        while(takenPositions[candidatePosition]) {
            candidatePosition++;
        }
        ch->variables[candidatePosition] = unusedElements[0];
    }

    // Remove temporary arrays
    delete[] unusedElements;
    delete[] takenPositions;

	return true;
}

}
