#include "CartesianCrxUniform.h"
#include "Cartesian_c.h"

namespace Cartesian {

void CartesianCrxUniform::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.uniform", (voidP) new double(0), ECF::DOUBLE);
}


bool CartesianCrxUniform::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.uniform");
	probability_ = *((double*)sptr.get());
	return true;
}


bool CartesianCrxUniform::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	Cartesian* p1 = (Cartesian*) (gen1.get());
	Cartesian* p2 = (Cartesian*) (gen2.get());
	Cartesian* ch = (Cartesian*) (child.get());

	// cross function nodes
	int randomParentChooser;
	for(uint i = 0; i < p1->nodes_.size(); i++) {
		randomParentChooser = state_->getRandomizer()->getRandomInteger(0, 1);
		if (randomParentChooser) 
			ch->nodes_[i] = p1->nodes_[i];
		else
			ch->nodes_[i] = p2->nodes_[i];
	}

	// cross outputs
	for (uint i = 0; i < p1->nOutputs_; i++) {
		randomParentChooser = state_->getRandomizer()->getRandomInteger(0, 1);
		if (randomParentChooser)
			ch->outputs_[i] = p1->outputs_[i];
		else
			ch->outputs_[i] = p2->outputs_[i];
	}

	return true;
}

}

