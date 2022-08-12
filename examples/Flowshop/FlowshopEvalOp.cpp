#include <ECF/ECF.h>
#include "FlowshopEvalOp.h"


void FlowshopEvalOp::registerParameters(StateP state)
{
	state->getRegistry()->registerEntry("flowshop.infile", (voidP) (new std::string), ECF::STRING);
}


bool FlowshopEvalOp::initialize(StateP state)
{
	state_ = state;

	if(!state->getRegistry()->isModified("flowshop.infile")) {
		state->getLogger()->log(1, "Error: no input file defined! (parameter 'flowshop.infile'");
		return false;
	}

	voidP sptr = state->getRegistry()->getEntry("flowshop.infile"); // get parameter value
	std::string filePath = *((std::string*) sptr.get()); // convert from voidP to user defined type

	// generate reusable objects from the original code
	problemInstance = ::newProblem(filePath.c_str());
	solutionInstance = ::allocSolution(problemInstance);
	::randomSolution(solutionInstance);
	solutionInstance2 = ::allocSolution(problemInstance);
	::randomSolution(solutionInstance2);
	moveInstance = ::allocMove(problemInstance);
	psInstance = ::allocPathState(problemInstance);
	
	// adjust Permutation genotype size
	state->getRegistry()->modifyEntry("Permutation.size", (voidP) new uint(problemInstance->n));

	// reinitialize population with updated size
	state->getPopulation()->initialize(state);
	
	return true;
}


void FlowshopEvalOp::populateSolutionInstance(IndividualP ind, struct solution* s)
{
	// get Permutation genotype from the individual
	Permutation::Permutation* perm = (Permutation::Permutation*) ind->getGenotype().get();
        
	// populate solution instance
	for(uint i = 0; i < perm->getSize(); i++)
		s->data[i] = perm->variables[i];
}


void FlowshopEvalOp::readSolutionInstance(IndividualP ind, struct solution* s)
{
	// get Permutation genotype from the individual
	Permutation::Permutation* perm = (Permutation::Permutation*) ind->getGenotype().get();
        
	// read solution instance
	for(uint i = 0; i < perm->getSize(); i++)
		perm->variables[i] = s->data[i];
}


MoveP FlowshopEvalOp::randomMove(IndividualP ind)
{
	populateSolutionInstance(ind, solutionInstance);
	FlowshopMove* m (new FlowshopMove(moveInstance));
	::randomMove(m->v, solutionInstance);
	return (MoveP) m;
}


bool FlowshopEvalOp::applyMove(IndividualP& ind, MoveP move)
{
	FlowshopMove* m = (FlowshopMove*) move.get();
	populateSolutionInstance(ind, solutionInstance);
	::applyMove(solutionInstance, m->v);
	readSolutionInstance(ind, solutionInstance);
	return true;
}


PathP FlowshopEvalOp::initPathTo(IndividualP ind1, IndividualP ind2)
{
	populateSolutionInstance(ind1, solutionInstance);
	populateSolutionInstance(ind2, solutionInstance2);
	::initPathTo(psInstance, solutionInstance, solutionInstance2);
	FlowshopPath* path (new FlowshopPath(psInstance));
    return (PathP) path; 
}


int FlowshopEvalOp::getPathLength(PathP path)
{
	FlowshopPath* p = (FlowshopPath*) path.get();
	return ::getPathLength(p->ps);
}


MoveP FlowshopEvalOp::nextRandomMove(PathP path)
{
	FlowshopMove* m (new FlowshopMove(moveInstance));
	::nextRandomMove(m->v, psInstance);
	return (MoveP) m;
}


FitnessP FlowshopEvalOp::evaluate(IndividualP individual)
{
    // minimization
    FitnessP fitness (new FitnessMin);

	populateSolutionInstance(individual, solutionInstance);

	// evaluate
	double value = ::getObjectiveValue(solutionInstance);

    fitness->setValue(value);

    return fitness;
}
