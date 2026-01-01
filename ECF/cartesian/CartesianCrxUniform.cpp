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
	//std::cout << "Cartesian cross unifrom" << std::endl;
	Cartesian* p1 = (Cartesian*) (gen1.get());
	Cartesian* p2 = (Cartesian*) (gen2.get());
	Cartesian* ch = (Cartesian*) (child.get());

	int randomParentChooser;
	for(unsigned long i = 0; i < p1->size(); i++) {
		randomParentChooser = state_->getRandomizer()->getRandomInteger(0, 1);
		switch (randomParentChooser) {
			case 0:
				ch->at(i) = p1->at(i);
				break;
			default:
				ch->at(i) = p2->at(i);
				break;
		}
	}

	return true;
}

}

