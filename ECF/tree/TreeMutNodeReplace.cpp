#include "../ECF_base.h"
#include "Tree.h"
#include "TreeMutNodeReplace.h"
#include <stdio.h>


namespace Tree
{

void TreeMutNodeReplace::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "mut.nodereplace", (voidP) new double(0), ECF::DOUBLE);
}


bool TreeMutNodeReplace::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "mut.nodereplace");
	probability_ = *((double*)sptr.get());
	return true;
}


bool TreeMutNodeReplace::mutate(GenotypeP gene)
{
	Tree* tree = (Tree*) (gene.get());

	// select random node in tree
	uint origNodeIndex = state_->getRandomizer()->getRandomInteger((int) tree->size());
	std::string origNodeName = tree->at(origNodeIndex)->primitive_->getName();
	int origNodeNumberOfArguments = tree->at(origNodeIndex)->primitive_->getNumberOfArguments();

	// try to select a primitive different from selected node's, with the same number of arguments
	PrimitiveP primitive;
	uint tries = 0;
	do {
		primitive = tree->primitiveSet_->getRandomPrimitive();
		tries++;
	} while((origNodeName == primitive->getName()
		|| origNodeNumberOfArguments != primitive->getNumberOfArguments()) && tries < 4);

	if(origNodeName == primitive->getName() || origNodeNumberOfArguments != primitive->getNumberOfArguments()) {
		ECF_LOG(state_, 5, "TreeMutNodeReplace not successful.");
		return false;
	}

	tree->at(origNodeIndex)->setPrimitive(primitive);

	std::stringstream log;
	log << "TreeMutNodeReplace successful (oldNode = " << origNodeName << ", ";
	log << "newNode = " << primitive->getName() << ")";
	ECF_LOG(state_, 5, log.str());

	return true;
}

}