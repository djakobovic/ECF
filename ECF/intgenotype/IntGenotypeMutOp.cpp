#include "../ECF_base.h"
#include <cmath>
#include "IntGenotype.h"

namespace IntGenotype
{
	
		bool IntGenotypeMutOp::mutate(GenotypeP gene)
		{
			IntGenotype* gen = (IntGenotype*) (gene.get());

			int a = state_->getRandomizer()->getRandomInteger(gen->intValues.size());
			gen->intValues[a] = state_->getRandomizer()->getRandomInteger(gen->getLBound(), gen->getUBound());

			return true;
		}
	
}