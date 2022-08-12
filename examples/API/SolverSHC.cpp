#include "ECF/ECF_base.h"
#include "SolverSHC.h"


SolverSHC :: SolverSHC()
{
	name_ = "SolverSHC";
}


bool SolverSHC :: advanceGeneration(StateP state, DemeP deme)
{
	// generate and apply move on every individual
	for(uint i = 0; i < deme->size(); i++) {
		IndividualP copy = (IndividualP) deme->at(i)->copy();
		MoveP move = getProblem()->randomMove(copy);
		getProblem()->applyMove(copy, move);
		evaluate(copy);
		// replace if better or equal
		if(!deme->at(i)->getFitness()->isBetterThan(copy->getFitness()))
			replaceWith(deme->at(i), copy);
	}

	return true;
}
