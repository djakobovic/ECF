#include <cmath>
#include <ecf/ECF.h>
#include <iostream>
#include <random>
#include "MutScale.h"
#include "../NeuralNetwork.h"



	void MutScale::registerParameters(StateP state)
	{
		myGenotype_->registerParameter(state, "mut.gaussall", (voidP) new double(0), ECF::DOUBLE);
	}

	bool MutScale::initialize(StateP state)
	{
		voidP sptr = myGenotype_->getParameterValue(state, "mut.gaussall");
		probability_ = *((double*)sptr.get());

		return true;
	}

	/* scaling the weight so that it doesn't change as radically*/
	bool MutScale::mutate(GenotypeP gene)
	{
		NeuralNetwork* FP = (NeuralNetwork*)(gene.get());

		std::random_device rd;
		std::mt19937 eng(rd());
		std::uniform_int_distribution<> distrWeight(0, FP->realValue.size() - 1);
		std::uniform_real_distribution<> distrScale(0.5, 1.5);

		FP->realValue[distrWeight(eng)] *= distrScale(eng);

		return true;
	}
