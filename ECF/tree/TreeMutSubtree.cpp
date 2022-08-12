#include "../ECF_base.h"
#include "Tree.h"
#include "TreeMutSubtree.h"
#include <stdio.h>


namespace Tree
{

void TreeMutSubtree::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "mut.subtree", (voidP) new double(0), ECF::DOUBLE);
}


bool TreeMutSubtree::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "mut.subtree");
	probability_ = *((double*)sptr.get());
	return true;
}


bool TreeMutSubtree::mutate(GenotypeP gene)
{
	Tree* tree = (Tree*) (gene.get());

	// try to select random node in tree which is not a terminal and which is not on maxDepth
	// this is a size-fair operator (depth condition)
	uint chosenNode;
	uint chosenNodeSubtreeSize;
	uint chosenNodeDepth;
	uint tries = 0;
	do {
		chosenNode = state_->getRandomizer()->getRandomInteger((int) tree->size());
		chosenNodeSubtreeSize = tree->at(chosenNode)->size_;
		chosenNodeDepth = tree->at(chosenNode)->depth_;
		tries++;
	} while((/*chosenNodeSubtreeSize == 1 || */chosenNodeDepth == tree->maxDepth_) && tries < 4);

	if(tries == 4) {
		ECF_LOG(state_, 5, "TreeMutSubtree not successful.");
		return false;
	}

	// first of all, make a copy of tree and clear the original
	Tree* copyTree = tree->copy();
	tree->clear();

	std::stringstream log;
	log << "TreeMutSubtree successful (";

	uint i = 0;

	// copy all nodes before chosen subtree to original
	for( ; i < chosenNode; i++) {
		NodeP node = static_cast<NodeP> (new Node(copyTree->at(i)->primitive_));
		tree->addNode(node);
	}

	log << "mutatedSubtree(at depth " << chosenNodeDepth << ") = ";
	for( ; i < chosenNode + chosenNodeSubtreeSize; i++) {
		// this node is an element of the chosen subtree
		log << copyTree->at(i)->primitive_->getName() << " ";
	}

	// generate new subtree with same primitives as original tree
	// maxDepth for that new tree is original tree->maxDepth_ - chosenNodeDepth
	Tree* newTree = tree->copy();
	newTree->clear();
	newTree->initMinDepth_ = 0;
	newTree->initMaxDepth_ = copyTree->maxDepth_ - chosenNodeDepth;

	if(state_->getRandomizer()->getRandomInteger(0, 1) % 2 == 0)
		newTree->fullBuild(copyTree->primitiveSet_);
	else
		newTree->growBuild(copyTree->primitiveSet_);

	log << ", newSubtree = ";
	// copy all nodes from newTree to tree
	for(uint j = 0; j < newTree->size(); j++) {
		NodeP node = static_cast<NodeP> (new Node(newTree->at(j)->primitive_));
		tree->addNode(node);
		log << node->primitive_->getName() << " ";
	}
	log << ")";

	// copy all nodes after chosen subtree to original
	for( ; i < copyTree->size(); i++) {
		NodeP node = static_cast<NodeP> (new Node(copyTree->at(i)->primitive_));
		tree->addNode(node);
	}

	tree->update();
	delete copyTree;
	delete newTree;

	ECF_LOG(state_, 5, log.str());

	return true;
}

}