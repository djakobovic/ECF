#include "CartesianCrxOnePoint.h"
#include "Cartesian_c.h"
#include "FunctionSet.h"

namespace Cartesian {

	void CartesianCrxOnePoint::registerParameters(StateP state)
	{
		myGenotype_->registerParameter(state, "crx.onepoint", (voidP) new double(0), ECF::DOUBLE);
	}


	bool CartesianCrxOnePoint::initialize(StateP state)
	{
		voidP sptr = myGenotype_->getParameterValue(state, "crx.onepoint");
		probability_ = *((double*)sptr.get());
		return true;
	}


	bool CartesianCrxOnePoint::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
	{
		Cartesian* p1 = (Cartesian*) (gen1.get());
		Cartesian* p2 = (Cartesian*) (gen2.get());
		Cartesian* ch = (Cartesian*) (child.get());

		uint nNodes = (uint) p1->nodes_.size() - p1->nInputs_;

		if (nNodes < 2) {
			// copy all from 1st parent
			ch->nodes_ = p1->nodes_;
			ch->outputs_ = p1->outputs_;
			return true;
		}

		uint cutoff = p1->nInputs_ + state_->getRandomizer()->getRandomInteger(1, nNodes - 1);

		for(uint i = 0; i < cutoff; i++) {
			ch->nodes_[i] = p1->nodes_[i];
		}
		for(uint i = cutoff; i < p2->nodes_.size(); i++) {
			ch->nodes_[i] = p2->nodes_[i];
		}
		// output nodes taken from the 2nd parent
		for (uint i = 0; i < p2->outputs_.size(); i++)
			ch->outputs_[i] = p2->outputs_[i];

		return true;
	}

}
