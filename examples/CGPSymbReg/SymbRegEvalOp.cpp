#include <cmath>
#include "ECF/ECF.h"
#include "SymbRegEvalOp.h"
#include <sstream>
#include <fstream>

#include "CGP/Cartesian.h"
#include "CGP/Cartesian_c.h"


// called only once, before the evolution - generates training data
bool CGPSymbRegEvalOp::initialize(StateP state)
{
	nSamples_ = 10;
	inputs_.resize(nSamples_);
	x1.clear(); x2.clear(); x3.clear();
	y1.clear(); y2.clear(); y3.clear();

	for(uint i = 0; i < nSamples_; i++) {
		x1.push_back(i + 1);
		x2.push_back(i + 5);
		x3.push_back(nSamples_ - i);

		inputs_[i].resize(3);
		inputs_[i][0] = x1[i];
		inputs_[i][1] = x2[i];
		inputs_[i][2] = x3[i];

		y1.push_back(x1[i] + x2[i]*x1[i] - x3[i]);
		y2.push_back(x1[i] * x3[i]);
		y3.push_back(x1[i] - x2[i] + sin(x3[i]));
	}
	for(uint i = 0; i < x1.size(); ++i) {
		std::cout << "X1: " << x1[i] << " X2: " << x2[i] << " X3: " << x3[i] << std::endl;
	}
	for(uint i = 0; i < y1.size(); ++i) {
		std::cout << "Y1: " << y1[i] << " Y2: " << y2[i] << " Y3: " << y3[i] << std::endl;
	}

	return true;
}


FitnessP CGPSymbRegEvalOp::evaluate(IndividualP individual)
{
	FitnessP fitness (new FitnessMin);
	Cartesian::Cartesian* cartesian = (Cartesian::Cartesian*) individual->getGenotype().get();

	double currentFitness = 0;
	std::vector<double> result;

	for(uint i = 0; i < nSamples_; i++)	{
		cartesian->evaluate(inputs_[i], result);

		currentFitness += fabs(result[0] - y1[i]);
		currentFitness += fabs(result[1] - y2[i]);
		//currentFitness += fabs(result[2] - y3[i]);
	}

	fitness->setValue(currentFitness);
	return fitness;
}
