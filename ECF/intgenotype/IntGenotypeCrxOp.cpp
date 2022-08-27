#include "../ECF_base.h"
#include <cmath>
#include "IntGenotype.h"

namespace IntGenotype
{
	
		bool IntGenotypeCrxOp::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child) {

			IntGenotype* p1 = (IntGenotype*)(gen1.get());
			IntGenotype* p2 = (IntGenotype*)(gen2.get());
			IntGenotype* ch = (IntGenotype*)(child.get());

			uint dimensionCrs = state_->getRandomizer()->getRandomInteger((int)p1->intValues.size());
			switch (state_->getRandomizer()->getRandomInteger(0, 1)) {
			case 0: for (uint i = 0; i < dimensionCrs; i++) {
						ch->intValues[i] = p1->intValues[i];
			}
					for (uint i = dimensionCrs; i < p2->intValues.size(); i++) {
						ch->intValues[i] = p2->intValues[i];
					}
					break;
			case 1: for (uint i = 0; i < dimensionCrs; i++) {
						ch->intValues[i] = p2->intValues[i];
			}
					for (uint i = dimensionCrs; i < p1->intValues.size(); i++) {
						ch->intValues[i] = p1->intValues[i];
					}
			}

			return true;
		}
	
}