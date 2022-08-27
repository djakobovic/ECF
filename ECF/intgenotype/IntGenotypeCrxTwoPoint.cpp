#include "../ECF_base.h"
#include <cmath>
#include "IntGenotype.h"

namespace IntGenotype
{
	
		bool IntGenotypeCrxTwoPoint::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child) {

			IntGenotype* p1 = (IntGenotype*)(gen1.get());
			IntGenotype* p2 = (IntGenotype*)(gen2.get());
			IntGenotype* ch = (IntGenotype*)(child.get());
			
			uint dimensionCrs1 = state_->getRandomizer()->getRandomInteger((int)p1->intValues.size());
			uint dimensionCrs2 = dimensionCrs1, tmp;

			if(p1->intValues.size() > 1)
				do {
					dimensionCrs2 = state_->getRandomizer()->getRandomInteger(p1->intValues.size());
				} while (dimensionCrs1 == dimensionCrs2);

			if (dimensionCrs1 > dimensionCrs2)
			{
				tmp = dimensionCrs1;
				dimensionCrs1 = dimensionCrs2;
				dimensionCrs2 = tmp;
			}

			switch (state_->getRandomizer()->getRandomInteger(0, 1)) {
			case 0: for (uint i = 0; i < dimensionCrs1; i++) {
						ch->intValues[i] = p1->intValues[i];
			}
					for (uint i = dimensionCrs1; i < dimensionCrs2; i++) {
						ch->intValues[i] = p2->intValues[i];
					}
					for (uint i = dimensionCrs2; i < p1->intValues.size(); i++) {
						ch->intValues[i] = p1->intValues[i];
					}
					break;
			case 1: for (uint i = 0; i < dimensionCrs1; i++) {
						ch->intValues[i] = p2->intValues[i];
			}
					for (uint i = dimensionCrs1; i < dimensionCrs2; i++) {
						ch->intValues[i] = p1->intValues[i];
					}
					for (uint i = dimensionCrs2; i < p2->intValues.size(); i++) {
						ch->intValues[i] = p2->intValues[i];
					}
			}
	
			return true;
		}
	
}