#ifndef KnapsackEvalOp_h
#define KnapsackEvalOp_h

#include "../API/Problem.h"


class KnapsackMove : public Move
{
public:
	int data;
};


class KnapsackPath : public Path
{
public:
    std::vector<int> pos;        /* indices of the bits in which the solutions differ */
    int n;
    int distance;
};


class KnapsackEvalOp : public Problem
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

	struct problem* problemInstance;
	struct solution* solutionInstance;

};
typedef boost::shared_ptr<KnapsackEvalOp> KnapsackEvalOpP;

#endif KnapsackEvalOp_h