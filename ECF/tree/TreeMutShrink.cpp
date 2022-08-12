#include "../ECF_base.h"
#include "Tree.h"
#include "TreeMutShrink.h"
#include <stdio.h>


namespace Tree
{

void TreeMutShrink::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "mut.shrink", (voidP) new double(0), ECF::DOUBLE);
}


bool TreeMutShrink::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "mut.shrink");
	probability_ = *((double*)sptr.get());
	return true;
}


bool TreeMutShrink::mutate(GenotypeP gene)
{
	Tree* tree = (Tree*) (gene.get());

	// try to select random node in tree which is not a terminal
	// (it is silly to shrink just a terminal :))
	uint chosenNode;
	uint chosenNodeSubtreeSize;
	uint tries = 0;
	do {
		chosenNode = state_->getRandomizer()->getRandomInteger((int) tree->size());
		chosenNodeSubtreeSize = tree->at(chosenNode)->size_;
		tries++;
	} while(chosenNodeSubtreeSize == 1 && tries < 4);

	if(chosenNodeSubtreeSize == 1) {
		ECF_LOG(state_, 5, "TreeMutShrink not successful.");
		return false;
	}

	// first of all, make a copy and clear the original
	Tree* copyTree = tree->copy();
	tree->clear();

	std::stringstream log;
	log << "TreeMutShrink successful (";

	uint i = 0;

	// copy all nodes before chosen subtree to original
	for( ; i < chosenNode; i++) {
		NodeP node = static_cast<NodeP> (new Node(copyTree->at(i)->primitive_));
		tree->addNode(node);
	}

	log << "shrinkedSubtree = ";
	for( ; i < chosenNode + chosenNodeSubtreeSize; i++) {
		// these nodes are skipped because they are elements of the chosen subtree
		log << copyTree->at(i)->primitive_->getName() << " ";
	}

	// chosen subtree is shrinked to a random terminal
	Node* node = new Node;
	node->setPrimitive(copyTree->primitiveSet_->getRandomTerminal());
	tree->addNode(node);
	log << ", shrinkedTo = " << node->primitive_->getName() << ")";

	// copy all nodes after chosen subtree to original
	for( ; i < copyTree->size(); i++) {
		NodeP node = static_cast<NodeP> (new Node(copyTree->at(i)->primitive_));
		tree->addNode(node);
	}

	tree->update();
	delete copyTree;

	ECF_LOG(state_, 5, log.str());

	return true;
}

}