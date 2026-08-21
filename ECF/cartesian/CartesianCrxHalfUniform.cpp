#include "CartesianCrxHalfUniform.h"
#include "Cartesian_c.h"

namespace Cartesian {

void CartesianCrxHalfUniform::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.halfuniform", (voidP) new double(0), ECF::DOUBLE);
}


bool CartesianCrxHalfUniform::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.halfuniform");
	probability_ = *((double*)sptr.get());
	return true;
}


bool CartesianCrxHalfUniform::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	Cartesian* p1 = (Cartesian*) (gen1.get());
	Cartesian* p2 = (Cartesian*) (gen2.get());
	Cartesian* ch = (Cartesian*) (child.get());

	// copy all from 1st parent
	ch->nodes_ = p1->nodes_;
	ch->outputs_ = p1->outputs_;

	// cross function nodes
	int geneSwitch;
	for (uint i = 0; i < p1->nodes_.size(); i++) {
		geneSwitch = state_->getRandomizer()->getRandomInteger(0, 1);
		if (geneSwitch)
			ch->nodes_[i] = p2->nodes_[i];
	}

	// cross outputs
	for (uint i = 0; i < p1->nOutputs_; i++) {
		geneSwitch = state_->getRandomizer()->getRandomInteger(0, 1);
		if (geneSwitch)
			ch->outputs_[i] = p2->outputs_[i];
	}

	return true;
}

}

