#include "../ECF_base.h"
#include "Tree.h"
#include "TreeMutHoist.h"
#include <stdio.h>


namespace Tree
{

void TreeMutHoist::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "mut.hoist", (voidP) new double(0), ECF::DOUBLE);
}


bool TreeMutHoist::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "mut.hoist");
	probability_ = *((double*)sptr.get());
	return true;
}


bool TreeMutHoist::mutate(GenotypeP gene)
{
	Tree* tree = (Tree*) (gene.get());
	
	// first of all, make a copy of tree and clear the original
	Tree* copyTree = tree->copy();
	tree->clear();
	
	// select random node in copied tree, it will be the root node for new tree
	uint chosenNode = state_->getRandomizer()->getRandomInteger((int) copyTree->size());
	uint chosenNodeSubtreeSize = copyTree->at(chosenNode)->size_;
	
	// copy chosen subtree to original
	for(uint i = 0; i < chosenNodeSubtreeSize; i++) {
		NodeP node = static_cast<NodeP> (new Node(copyTree->at(chosenNode + i)->primitive_));
		tree->addNode(node);
	}
	
	tree->update();
	delete copyTree;
	
	std::stringstream log;
	log << "TreeMutHoist successful (hoisted subtree = ";
	for(uint i = 0; i < tree->size(); i++)
		log << tree->at(i)->primitive_->getName() << " ";
	log << ")";
	ECF_LOG(state_, 5, log.str());

	return true;
}

}