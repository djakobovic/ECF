#include "../ECF_base.h"
#include "Binary.h"

namespace Binary
{

void BinaryCrsReducedSurrogate::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.reducedsurrogate", (voidP) new double(0), ECF::DOUBLE);
}


bool BinaryCrsReducedSurrogate::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.reducedsurrogate");
	probability_ = *((double*)sptr.get());
	return true;
}


bool BinaryCrsReducedSurrogate::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
    Binary* p1 = (Binary*) (gen1.get());
	Binary* p2 = (Binary*) (gen2.get());
	Binary* ch = (Binary*) (child.get());
	
    for (uint dimension = 0; dimension < p1->variables.size(); dimension++) {
		
		std::vector<int> reduced;
		for(uint i = 0; i < p1->getNumBits(); i++) {
			if (p1->variables[dimension][i] != p2->variables[dimension][i]) 
				reduced.push_back((int)i);
		}
		
		uint bitCrs;
		int	location=-1;
		
		if (reduced.size() > 0 )	{
			location = state_->getRandomizer()->getRandomInteger((int) reduced.size());
			bitCrs=reduced[location];
		}

		uint parent = state_->getRandomizer()->getRandomInteger(0, 1);
		
		for(uint i = 0; i < p1->getNumBits(); i++) {
			if (p1->variables[dimension][i] == p2->variables[dimension][i]) 
				ch->variables[dimension][i] = p1->variables[dimension][i];
			else	{
				if (parent==0)	{
					if (i < bitCrs)
						ch->variables[dimension][i] = p1->variables[dimension][i];
					else
						ch->variables[dimension][i] = p2->variables[dimension][i];
				}
				else	{
					if (i < bitCrs)
						ch->variables[dimension][i] = p2->variables[dimension][i];
					else
						ch->variables[dimension][i] = p1->variables[dimension][i];
				}	
			}
		}
	}

	ch->update();

	return true;
}

}