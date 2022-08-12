#include "ECF_base.h"
#include "ECF_macro.h"
#include "AlgSteadyStateTournament.h"
#include "SelRandomOp.h"
#include "SelWorstOp.h"


SteadyStateTournament::SteadyStateTournament()
{
	// define algorithm name
	name_ = "SteadyStateTournament";

	// create selection operators needed
	// in this case, SelRandomOp and SelWorstOp
	selRandomOp = static_cast<SelectionOperatorP> (new SelRandomOp);
	selWorstOp = static_cast<SelectionOperatorP> (new SelWorstOp);
}


void SteadyStateTournament::registerParameters(StateP state)
{
	registerParameter(state, "tsize", (voidP) new uint(3), ECF::UINT,
		"tournament size (individuals selected randomly, worst one eliminated)");
}


bool SteadyStateTournament::initialize(StateP state)
{
	// initialize all operators
	selRandomOp->initialize(state);
	selWorstOp->initialize(state);

	// read parameter values 
	voidP tsizep = getParameterValue(state, "tsize");
	nTournament_ = *((uint*) tsizep.get());

	if(nTournament_ < 3) {
		ECF_LOG(state, 1, "Error: SteadyStateTournament algorithm requires minimum tournament size of 3!");
		throw "";
	}

	return true;
}


bool SteadyStateTournament::advanceGeneration(StateP state, DemeP deme)
{
	//! this algorithm: one _generation_ is 'deme->size()' _iterations_
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

		// crossover the first two (remaining) individuals in the tournament
		mate(tournament[0], tournament[1], worst);

		// perform mutation on new individual
		mutate(worst);

		// create new fitness
		evaluate(worst);
		ECF_LOG(state, 5, "New individual: " + worst->toString());
	}

	return true;
}
