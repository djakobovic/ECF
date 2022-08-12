#include <cmath>
#include <ecf/ECF.h>
#include <iostream>
#include <random>
#include "MutGauss.h"
#include "../NeuralNetwork.h"


void MutGauss::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "mut.gauss", (voidP) new double(0), ECF::DOUBLE);
}

bool MutGauss::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "mut.gauss");
	probability_ = *((double*)sptr.get());

	return true;
}

bool MutGauss::mutate(GenotypeP gene)
{
	NeuralNetwork* FP = (NeuralNetwork*)(gene.get());

//	std::default_random_engine generator(std::random_device{}());
	std::default_random_engine generator;

	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(0, FP->realValue.size() - 1);

	int dimension = distr(eng);
	/*deviation is calculated as one tenth of upper bound*/
	std::normal_distribution<double> distribution(0, FP->getUBound() / 10);
	double number = FP->realValue[dimension] + distribution(generator);

	if (number > FP->getUBound()) {
		FP->realValue[dimension] = FP->getUBound();
	}
	else if (number < FP->getLBound()) {
		FP->realValue[dimension] = FP->getLBound();
	}
	else {
		FP->realValue[dimension] = number;
	}
	return true;
}
