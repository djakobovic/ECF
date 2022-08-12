#include<iostream>
#include "ECF/ECF.h"
#include "KnapsackEvalop.h"
#include "../API/SolverSHC.h"
#include "../API/SolverTournamentEA.h"
#include "../API/SolverEvolutionStrategy.h"


int main(int argc, char** argv)
{
	StateP state (new State);

	// add dynamic components
	state->addAlgorithm((AlgorithmP) new SolverSHC);
	state->addAlgorithm((AlgorithmP) new SolverTournamentEA);
	state->addAlgorithm((AlgorithmP) new SolverEvolutionStrategy);

	// set problem (evaluator)
	state->setEvalOp(new KnapsackEvalOp());

	state->initialize(argc, argv);
	state->run();

	return 0;
}