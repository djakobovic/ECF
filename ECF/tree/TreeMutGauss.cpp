#include "../ECF_base.h"
#include "Tree.h"
#include "TreeMutGauss.h"
#include <stdio.h>
#include <time.h>


namespace Tree
{

void TreeMutGauss::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "mut.gauss", (voidP) new double(0), ECF::DOUBLE);
}


bool TreeMutGauss::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "mut.gauss");
	probability_ = *((double*)sptr.get());
	std::random_device rd;
	std::mt19937 rng(rd());
	engine_ = rng;
	//engine_.seed((uint32_t) time(NULL));

	return true;
}


bool TreeMutGauss::mutate(GenotypeP gene)
{
	Tree* tree = (Tree*) (gene.get());
	
	// try to select ERC node of type double
	uint iNode;
	uint tries = 0;
	std::string name;
	do {
		iNode = state_->getRandomizer()->getRandomInteger((int) tree->size());
		tries++;
	} while((name = tree->at(iNode)->primitive_->getName()).substr(0, 2) != DBL_PREFIX && tries < 4);

	if(name.substr(0, 2) != DBL_PREFIX)	{
		ECF_LOG(state_, 5, "TreeMutGauss not successful.");
		return false;
	}

	double oldValue;
	PrimitiveP oldPrim = tree->at(iNode)->primitive_;
	tree->at(iNode)->primitive_->getValue(&oldValue);
	std::string oldName = tree->at(iNode)->primitive_->getName();

	// generate Gauss noise offset and add it
	// TODO: parametrize distribution!
	std::normal_distribution<double> N(0, 1);

	// e.g. http://www.codepedia.com/1/CppBoostRandom
	// TODO: preserve state
	//boost::lagged_fibonacci607 engine(state_->getRandomizer()->getRandomInteger(100000) + 1);

	//double offset = N.operator () <boost::lagged_fibonacci607>(engine_);
	double offset = N(engine_);
	double newValue = oldValue + offset;

	// change double ERC value and name
	std::stringstream ss;
	ss << newValue;
	std::string newName;
	ss >> newName;
	newName = DBL_PREFIX + newName;

	oldPrim->setName(newName);
	oldPrim->setValue(&newValue);

	// new ERCs aren't stored in the PrimitiveSet

	std::stringstream log;
	log << "TreeMutGauss successful (oldNode = " << oldName << ", newNode = " << newName << ")";
	ECF_LOG(state_, 5, log.str());

	return true;
}

}