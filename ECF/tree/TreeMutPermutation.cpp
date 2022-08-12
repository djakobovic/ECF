#include "../ECF_base.h"
#include "Tree.h"
#include "TreeMutPermutation.h"
#include <stdio.h>


namespace Tree
{

void TreeMutPermutation::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "mut.permutation", (voidP) new double(0), ECF::DOUBLE);
}


bool TreeMutPermutation::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "mut.permutation");
	probability_ = *((double*)sptr.get());
	return true;
}


bool TreeMutPermutation::mutate(GenotypeP gene)
{
	Tree* tree = (Tree*) (gene.get());

	// try to select a node with >1 args
	uint iNode, nArgs;
	uint tries = 0;
	do {
		iNode = state_->getRandomizer()->getRandomInteger((int) tree->size());
		tries++;
	} while((nArgs = tree->at(iNode)->primitive_->getNumberOfArguments()) < 2 && tries < 4);

	if(nArgs < 2)
		return false;

	// build permutation vector
	std::vector<uint> permutation;
	permutation.resize(nArgs);

	for(uint i = 0; i < nArgs; i++) {
		permutation[i] = i;
	}
	uint ind1, ind2, temp;
	for(uint i = 0; i < nArgs - 1; i++) {
		ind1 = permutation[i];
		ind2 = state_->getRandomizer()->getRandomInteger((int) i, (int) nArgs - 1);
		temp = permutation[ind1];
		permutation[ind1] = permutation[ind2];
		permutation[ind2] = temp;
	}

	// record original children node's offsets
	std::vector<uint> offsets;
	offsets.push_back(1);
	for(uint i = 1; i < nArgs; i++) {
		offsets.push_back(offsets[i - 1] + tree->at(iNode + offsets[i - 1])->size_);
	}

	// reorder mutated nodes
	std::vector<NodeP> mutNodes;
	mutNodes.assign(tree->begin() + iNode, tree->begin() + iNode + tree->at(iNode)->size_);

	uint iCopiedTo = 1;
	for(uint iArg = 0; iArg < nArgs; iArg++) {
		uint nNodes = mutNodes[offsets[permutation[iArg]]]->size_;
		for(uint i = 0; i < nNodes; i++, iCopiedTo++) {
			tree->at(iNode + iCopiedTo) = mutNodes[offsets[permutation[iArg]] + i];
		}
	}

	return true;
}

}