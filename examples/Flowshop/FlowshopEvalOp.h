#ifndef FlowshopEvalOp_h
#define FlowshopEvalOp_h

#include "../API/Problem.h"
#include "flowshop.h"


class FlowshopMove : public Move
{
public:
	struct move* v;
	FlowshopMove(struct move* _v = 0)	
	{	v = _v;	}
};


class FlowshopPath : public Path
{
public:
    struct pathState* ps;
	FlowshopPath(struct pathState* _ps = 0)
	{	ps = _ps;	}
};


class FlowshopEvalOp : public Problem
{
public:
	StateP state_;

	// ECF methods
	void registerParameters(StateP);
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
typedef boost::shared_ptr<FlowshopEvalOp> FlowshopEvalOpP;

#endif FlowshopEvalOp_h