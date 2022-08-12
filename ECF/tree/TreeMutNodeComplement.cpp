#include "../ECF_base.h"
#include "Tree.h"
#include "TreeMutNodeComplement.h"
#include <stdio.h>


namespace Tree
{

void TreeMutNodeComplement::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "mut.nodecomplement", (voidP) new double(0), ECF::DOUBLE);
}


bool TreeMutNodeComplement::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "mut.nodecomplement");
	probability_ = *((double*)sptr.get());
	return true;
}


bool TreeMutNodeComplement::mutate(GenotypeP gene)
{
	Tree* tree = (Tree*) (gene.get());

	// try to select a random node whose complement is in primitive set
	uint tries = 0;
	uint index;
	std::string name, complementName;
	PrimitiveP complement;
	do {
		index = state_->getRandomizer()->getRandomInteger((int) tree->size());
		name = tree->at(index)->primitive_->getName();
		complementName = tree->at(index)->primitive_->getComplementName();
		
		complement = tree->primitiveSet_->getPrimitiveByName(complementName);
		tries++;
	} while(complement == PrimitiveP() && tries < 4);

	if(complement == PrimitiveP()) {
		ECF_LOG(state_, 5, "TreeMutNodeComplement not successful.");
		return false;
	}

	tree->at(index)->setPrimitive(complement);

	std::stringstream log;
	log << "TreeMutNodeComplement successful (oldNode = " << name << ", ";
	log << "newNode = " << complementName << ")";
	ECF_LOG(state_, 5, log.str());

	return true;
}

}