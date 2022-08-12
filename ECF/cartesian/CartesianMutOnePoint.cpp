#include "CartesianMutOnePoint.h"
#include <cmath>

namespace cart{

void CartesianMutOnePoint::registerParameters(StateP state)
{
	//TODO: ako ce se jednog dana koristiti vise operatora
	//myGenotype_->registerParameter(state, "mut.onepoint", (voidP) new double(0), DOUBLE);
	myGenotype_->registerParameter(state, "mut.onepoint.prob", (voidP) new double(0.001), ECF::DOUBLE);
}


bool CartesianMutOnePoint::initialize(StateP state)
{
	//TODO: ako ce se jednog dana koristiti vise operatora
	//voidP sptr = myGenotype_->getParameterValue(state, "mut.onepoint");
	//probability_ = *((double*)sptr.get());

	voidP sptr = myGenotype_->getParameterValue(state, "mut.onepoint.prob");
	mutProb_ = *((double*)sptr.get());

	useMutProb_ = false;
	if(myGenotype_->isParameterDefined(state, "mut.onepoint.prob"))
		useMutProb_ = true;

	return true;
}


bool CartesianMutOnePoint::mutate(GenotypeP gene)
{
	Cartesian* mut = (Cartesian*) (gene.get());

	//if mutation probability is used, then choose values in genotpye to be mutated depending on the
	//predefined mutation probability
	if (useMutProb_)
	{
		for (int i = 0; i < mut->size(); i++)
		{
			if (state_->getRandomizer()->getRandomInteger(0, 1) < mutProb_)
			{
				mutOneValue(mut, i);
			}
		}
	}
	//if mutation probability isn't used, then randomly choose one value in genotype to be mutated
	else
	{
		uint mutPoint = (uint)(state_->getRandomizer()->getRandomInteger(0, mut->size() - 1));

		mutOneValue(mut, mutPoint);
	}

	return true;
}

void CartesianMutOnePoint::mutOneValue(Cartesian *mut, int mutPoint)
{
	//value chosen to be mutated in either input connection or function token
	if (mutPoint < (mut->getNumOfRows() * mut->getNumOfCols() * (mut->getNumOfInputConn() + 1)))
	{
		//chosen value represents function token
		if (((mutPoint + 1) % (mut->getNumOfInputConn() + 1) == 0))
		{
			mut->at(mutPoint) = mut->randFunction();
		}
		//chosen value represents input connection
		else
		{
			double num = (double)mutPoint / (double)(mut->getNumOfRows() * (mut->getNumOfInputConn() + 1));
			//column in which input connection of current node is placed
			uint currCol = (uint)floor(num) + 1;
			mut->at(mutPoint) = mut->randInputConn(currCol - 1);
		}
	}
	//chosen value represents output
	else
	{
		mut->at(mutPoint) = mut->randOutput();
	}
}

}

