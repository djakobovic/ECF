#include "../ECF_base.h"
#include "Permutation.h"
#include <map>


namespace Permutation
{

void PermutationCrsUPMX::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.UPMX", (voidP) new double(0), ECF::DOUBLE);
}


bool PermutationCrsUPMX::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.UPMX");
	probability_ = *((double*)sptr.get());
	return true;
}

// A helper class for managment of inverse indexes
class IndexBackedPermutation {
    private:
        int * inverseIndexes;  // Owned by this class
        int * array;           // Just an outer reference
        int n;                 // Size of array to be indexed

    public:
        // Constructor. arr can be NULL; if not, it will be autoindexed.
        IndexBackedPermutation(int * arr, int n_) {
            array = arr;
            n = n_;
            inverseIndexes = new int[n];
            if(array!=NULL) {
                for(int i = 0; i < n; i++) {
                    inverseIndexes[array[i]] = i;
                }
            }
        }

        // Destructor.
        ~IndexBackedPermutation() {
            delete [] inverseIndexes;
        }

        // Setter for index for given value.
        void setIndexForValue(int value, int index) {
            inverseIndexes[value] = index;
        }

        // Getter for index of given value.
        int getIndexForValue(int value) {
            return inverseIndexes[value];
        }

        // Removes element which is in array at given position.
        // Internally, it swaps it with current last element,
        // updates index and decreases the size of collection by one.
        void remove(int* size, int pos) {
            *size = *size - 1;

            // swap elements
            int t = array[*size];
            array[*size] = array[pos];
            array[pos] = t;

            // swap element indexes
            t = inverseIndexes[array[pos]];
            inverseIndexes[array[pos]] = inverseIndexes[array[*size]];
            inverseIndexes[array[*size]] = t;
        }

        // Swaps element at positions pos1 and pos2 in given
        // permutation and updates indexes to reflect this change.
        void swap(Permutation *p, int pos1, int pos2) {
            // swap elements
            int t = p->variables[pos1];
            p->variables[pos1] = p->variables[pos2];
            p->variables[pos2] = t;

            // swap element indexes
            t = inverseIndexes[p->variables[pos1]];
            inverseIndexes[p->variables[pos1]] = inverseIndexes[p->variables[pos2]];
            inverseIndexes[p->variables[pos2]] = t;
        }
};


bool PermutationCrsUPMX::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	Permutation* p1 = (Permutation*) (gen1.get());
	Permutation* p2 = (Permutation*) (gen2.get());
	Permutation* ch = (Permutation*) (child.get());

    int capacity = (int) p1->getSize();

    // Create an array of legal positions for swap in child;
    // Initially, all positions are legal; later, illegal positions
    // will be moved to end and the collection size will be decreased
    int * legalPositions = new int[capacity];
    for(int i = 0; i < capacity; i++) {
        legalPositions[i] = i;
    }

    // Declare indexes for child and parent2;
    // Create index for legal positions (it will auto-initialize).
    IndexBackedPermutation idxCh(NULL, capacity);
    IndexBackedPermutation idxP2(NULL, capacity);
    IndexBackedPermutation idxLegal(legalPositions, capacity);

    // Make child a clone of parent1;
    // Initialize indexes for child and parent2
    for(int i = 0; i < capacity; i++) {
        ch->variables[i] = p1->variables[i];
        idxCh.setIndexForValue(p1->variables[i], i);
        idxP2.setIndexForValue(p2->variables[i], i);
    }

    // How many swaps we will try:
    int attempts = capacity / 3;

    // How many legal positions we actually have?
    int legalsCount = capacity;

    // Go n/3 times:
    for(int attempt = 0; attempt < attempts; attempt++) {
        // Pick one remaining position for parent1
        int rand = state_->getRandomizer()->getRandomInteger(legalsCount);
        int pos1 = legalPositions[rand];
        idxLegal.remove(&legalsCount, rand);

        // Find in parent2 the location of selected value:
        int value1 = ch->variables[pos1];
        int pos2 = idxP2.getIndexForValue(value1);

        // Now swap in child elements at locations pos1 and pos2:
        idxCh.swap(ch, pos1, pos2);

        // If pos2 is among currently legal for picking, forbid it too:
        if(idxLegal.getIndexForValue(pos2) < legalsCount) {
            idxLegal.remove(&legalsCount, idxLegal.getIndexForValue(pos2));
        }
    }

    delete [] legalPositions;

	return true;
}

}
