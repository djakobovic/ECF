#pragma once
#include "ECF/Algorithm.h"
#include "Problem.h"

//
// base Solver class
//
class Solver : public Algorithm
{
public:
	// access the user registered Problem class instance
	Problem* getProblem()
	{
		return ((Problem*) state_->getEvalOp().get());
	}

	// methods for checking the Problem provides API methods
	bool checkInitPathTo(IndividualP ind1, IndividualP ind2)
	{
		return ((Problem*) state_->getEvalOp().get())->initPathTo(ind1, ind2);
	}
	bool checkInitPathAwayFrom(IndividualP ind)
	{
		return ((Problem*) state_->getEvalOp().get())->initPathAwayFrom(ind);
	}
	bool checkNextRandomMove(PathP path)
	{
		return ((Problem*) state_->getEvalOp().get())->nextRandomMove(path);
	}
};
