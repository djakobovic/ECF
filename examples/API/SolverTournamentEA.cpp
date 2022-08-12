#include "ECF/ECF_base.h"
#include "ECF/SelRandomOp.h"
#include "ECF/SelWorstOp.h"
#include "SolverTournamentEA.h"


SolverTournamentEA::SolverTournamentEA()
{
	name_ = "SolverTournamentEA";

	// create selection operators needed
	selRandomOp = (SelectionOperatorP) (new SelRandomOp);
	selWorstOp = (SelectionOperatorP) (new SelWorstOp);
}


void SolverTournamentEA::registerParameters(StateP state)
{	
	registerParameter(state, "tsize", (voidP) new uint(3), ECF::UINT,
		"tournament size (individuals selected randomly, worst one eliminated)");
}


bool SolverTournamentEA::initialize(StateP state)
{
	selRandomOp->initialize(state);
	selWorstOp->initialize(state);

	// read parameter values 
	voidP tsizep = getParameterValue(state, "tsize");
	nTournament_ = *((uint*) tsizep.get());

	if(nTournament_ < 3) {
		ECF_LOG(state, 1, "Error: SolverTournamentEA requires minimum tournament size of 3!");
		throw "";
	}

	// check if the problem provides API methods
	getProblem()->registerParameters(state);
	getProblem()->initialize(state);
	IndividualP ind (new Individual(state));
	PathP path = getProblem()->initPathTo(ind, ind);
	uint l = getProblem()->getPathLength(path);
	MoveP move = getProblem()->nextRandomMove(path);
	if(path == 0 || l == -1 || move == 0) {
		ECF_LOG(state, 1, "Error: SolverTournamentEA requires initPathTo, getPathLength, nextRandomMove!");
		throw "";
	}
	return true;
}


bool SolverTournamentEA::advanceGeneration(StateP state, DemeP deme)
{
	//! for this algorithm, one _generation_ is 'deme->size()' _iterations_
	for(uint iIter = 0; iIter < deme->size(); iIter++) {

		ECF_LOG(state, 5, "Individuals in tournament: ");

		std::vector<IndividualP> tournament;
		for (uint i = 0; i < nTournament_; ++i) {
			// select a random individual for the tournament
			tournament.push_back(selRandomOp->select(*deme));
			ECF_LOG(state, 5, uint2str(i) + ": " + tournament[i]->toString());
		}

		// select the worst from the tournament
		IndividualP worst = selWorstOp->select(tournament);
		ECF_LOG(state, 5, "The worst from the tournament: " + worst->toString());

		// remove pointer to 'worst' individual from vector 'tournament'
		removeFrom(worst, tournament);

		// generate new individual, replace worst
		IndividualP child (tournament[0]->copy());
		replaceWith(worst, child);

		// geometric recombination of the first two (remaining) individuals in the tournament
		PathP path = getProblem()->initPathTo(tournament[0], tournament[1]);
		int l = getProblem()->getPathLength(path);
		if (l > 1) {
			l -= state->getRandomizer()->getRandomInteger(l/2) + 1;
			/* This depends on the current path length being updated by
				* nextRandomMove() and assumes that calling getPathLength() is
				* cheap. */
			while (getProblem()->getPathLength(path) > l) {
				MoveP move = getProblem()->nextRandomMove(path);
				getProblem()->applyMove(child, move);
			}
		}
		// (original code form ECF: crossover)
		//mate(tournament[0], tournament[1], worst);

		// single-step mutation of new individual
		if(state->getRandomizer()->getRandomDouble() <= mutation_->getIndMutProb()) {
			MoveP move = getProblem()->randomMove(child);
			getProblem()->applyMove(child, move);
		}
		// (original code from ECF: perform mutation on new individual)
		//mutate(worst);

		// create new fitness
		evaluate(child);
		ECF_LOG(state, 5, "New individual: " + child->toString());
	}

	return true;
}
