#include "../ECF_base.h"
#include "Binary.h"

namespace Binary
{

void BinaryCrsRandomRespectful::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.randomrespectful", (voidP) new double(0), ECF::DOUBLE);
}


bool BinaryCrsRandomRespectful::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.randomrespectful");
	probability_ = *((double*)sptr.get());
	return true;
}


bool BinaryCrsRandomRespectful::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
    Binary* p1 = (Binary*) (gen1.get());
	Binary* p2 = (Binary*) (gen2.get());
	Binary* ch = (Binary*) (child.get());

	double rand;
	typedef std::vector<uint> v_uint;

	v_uint similarity;

	for (uint dimension = 0; dimension < p1->variables.size(); dimension++) {
		for(uint i = 0; i < p1->getNumBits(); i++) {
			if (p1->variables[dimension][i] == p2->variables[dimension][i])
				similarity.push_back(p2->variables[dimension][i]);
			else
				similarity.push_back(2);
		}
	}

    for (uint dimension = 0; dimension < p1->variables.size(); dimension++) {
		for(uint i = 0; i < p1->getNumBits(); i++) {
			if (similarity[dimension*p1->getNumBits()+i]==1)
				ch->variables[dimension][i]= 1;
			else if (similarity[dimension*p1->getNumBits()+i]==0)
				ch->variables[dimension][i]= 0;
			else {
			    rand = state_->getRandomizer()->getRandomDouble();
				if (rand <= 0.5)
					ch->variables[dimension][i]= 1;
				else
					ch->variables[dimension][i]= 0;
			}
		}
	}

	ch->update();

	return true;
}

}