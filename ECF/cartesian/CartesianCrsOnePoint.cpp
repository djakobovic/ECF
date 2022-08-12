#include "CartesianCrsOnePoint.h"
#include "Cartesian_c.h"

namespace cart{

void CartesianCrsOnePoint::registerParameters(StateP state)
{
	//TODO: ako ce se jednog dana koristiti vise operatora
	//myGenotype_->registerParameter(state, "crx.onepoint", (voidP) new double(0), DOUBLE);
}


bool CartesianCrsOnePoint::initialize(StateP state)
{
	//TODO: ako ce se jednog dana koristiti vise operatora
	//voidP sptr = myGenotype_->getParameterValue(state, "crx.onepoint");
	//probability_ = *((double*)sptr.get());
	return true;
}


bool CartesianCrsOnePoint::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	Cartesian* p1 = (Cartesian*) (gen1.get());
	Cartesian* p2 = (Cartesian*) (gen2.get());
	Cartesian* ch = (Cartesian*) (child.get());

	//crossing point is any integer value in parents
	int crsPoint = state_->getRandomizer()->getRandomInteger(p1->size());

	ch->clear();
	//randomly choose left side of first parent or second parent
	if ((state_->getRandomizer()->getRandomInteger(0, 1)) == 0)
	{
		for (int i = 0; i < crsPoint; i++)
		{
			ch->push_back(p1->at(i));
		}
		for (int i = crsPoint; i < (int)p2->size(); i++)
		{
			ch->push_back(p2->at(i));
		}
	}
	else
	{
		for (int i = 0; i < crsPoint; i++)
		{
			ch->push_back(p2->at(i));
		}
		for (int i = crsPoint; i < (int)p1->size(); i++)
		{
			ch->push_back(p1->at(i));
		}
	}

	return true;
}

}

