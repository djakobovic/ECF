#ifndef NQueensEvalOp_h
#define NQueensEvalOp_h

#include "../API/Problem.h"
#include "nqueens.h"


class NQueensMove : public Move
{
public:
	struct move* v;
	NQueensMove()	{}
	NQueensMove(struct move* _v)	
	{	v = _v;	}
};


class NQueensPath : public Path
{
public:
    struct pathState* ps;
	NQueensPath()	{}
	NQueensPath(struct pathState* _ps)	
	{	ps = _ps;	}
};


class NQueensEvalOp : public Problem
{
public:
	StateP state_;

	// ECF methods
	bool initialize(StateP);
	FitnessP evaluate(IndividualP individual);

	// API methods
	MoveP randomMove(IndividualP);
	bool applyMove(IndividualP&, MoveP);
	PathP initPathTo(IndividualP, IndividualP);
	int getPathLength(PathP);
	MoveP nextRandomMove(PathP);

	// reusable objects from the original code
	struct problem* problemInstance;
	struct solution *solutionInstance, *solutionInstance2;
	struct move* moveInstance;
	struct pathState* psInstance;
	void populateSolutionInstance(IndividualP, struct solution*);
	void readSolutionInstance(IndividualP, struct solution*);

};
typedef boost::shared_ptr<NQueensEvalOp> NQueensEvalOpP;

#endif NQueensEvalOp_h