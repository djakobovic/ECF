#include "../ECF_base.h"
#include <cmath>
#include "IntGenotype.h"

namespace IntGenotype
{
	
		bool IntGenotypeCrxAverage::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child) {

			IntGenotype* p1 = (IntGenotype*)(gen1.get());
			IntGenotype* p2 = (IntGenotype*)(gen2.get());
			IntGenotype* ch = (IntGenotype*)(child.get());

			for (uint i = 0; i < p1->intValues.size(); i++) {
				ch->intValues[i] = (int) (0.5 * (p1->intValues[i]) + 0.5 * (p2->intValues[i]));
			}

			
			return true;
		}
	
}