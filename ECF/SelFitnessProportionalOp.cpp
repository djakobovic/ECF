#include "ECF_base.h"
#include "SelFitnessProportionalOp.h"
#include "SelBestOp.h"
#include "SelWorstOp.h"
#include <cmath>


bool SelFitnessProportionalOp::initialize(StateP state) {
	state_ = state;
	selPressure_ = 10;
	return true;
}


bool SelFitnessProportionalOp::setSelPressure(double selPressure)
{
	if(selPressure > 0){
		selPressure_ = selPressure;
		return true;
	}
	return false;
}


IndividualP SelFitnessProportionalOp::select(const std::vector<IndividualP>& pool)
{
	if(pool.empty())
		return IndividualP();

	SelBestOp best_;
	SelWorstOp worst_;
	IndividualP best = best_.select(pool);
	IndividualP worst = worst_.select(pool);
	double bestVal = best->fitness->getValue();
	double worstVal = worst->fitness->getValue();
	std::vector<double> howFit(pool.size(), 0.); 

	// check for uniform population
	if(fabs(bestVal - worstVal) < 0.000001)
		bestVal = worstVal + 1;

	howFit[0] = 1 + (selPressure_ - 1) * (pool[0]->fitness->getValue() - worstVal)/(bestVal - worstVal);
	for(uint i = 1; i<pool.size(); i++) {
		howFit[i] = 1 + (selPressure_ - 1) * (pool[i]->fitness->getValue() - worstVal)/(bestVal - worstVal);
		howFit[i] += howFit[i-1];
	}

	double randVal = state_->getRandomizer()->getRandomDouble() * howFit[howFit.size() - 1];
	uint chosen = 0;
	while(howFit[chosen] < randVal)
		chosen++;

	return pool[chosen];
}


std::vector<IndividualP> SelFitnessProportionalOp::selectMany(const std::vector<IndividualP>& pool, uint repeats)
{
	if(pool.empty())
		return pool;

	SelBestOp best_;
	SelWorstOp worst_;
	IndividualP best = best_.select(pool);
	IndividualP worst = worst_.select(pool);
	double bestVal = best->fitness->getValue();
	double worstVal = worst->fitness->getValue();
	std::vector<double> howFit(pool.size(), 0.);
	std::vector<IndividualP> selected;

	// check for uniform population
	if(fabs(bestVal - worstVal) < 0.000001)
		bestVal = worstVal + 1;

	howFit[0] = 1 + (selPressure_ - 1) * (pool[0]->fitness->getValue() - worstVal)/(bestVal - worstVal);
	for(uint i = 1; i<pool.size(); i++) {
		howFit[i] = 1 + (selPressure_ - 1) * (pool[i]->fitness->getValue() - worstVal)/(bestVal - worstVal);
		howFit[i] += howFit[i-1];
	}

	for(uint i = 0; i < repeats; i++) {
		double randVal = state_->getRandomizer()->getRandomDouble() * howFit[howFit.size() - 1];
		uint chosen = 0, begin = 0, end = (uint) howFit.size() - 1;
		while((begin + 1) < end) {
			chosen = (begin + end) / 2;
			if(howFit[chosen] > randVal)
				end = chosen;
			else
				begin = chosen;
		}
		if(howFit[begin] >= randVal)
			chosen = begin;
		else
			chosen = end;

//		while(howFit[chosen] < randVal)
//			chosen++;
		selected.push_back(pool[chosen]);
	}

	return selected;
}
