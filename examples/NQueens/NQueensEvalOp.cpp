#include <ECF/ECF.h>
#include "NQueensEvalOp.h"


bool NQueensEvalOp::initialize(StateP state)
{
	state_ = state;
	voidP sptr = state->getRegistry()->getEntry("Permutation.size");
	int problemSize = *((int*) sptr.get());

	// generate reusable objects from the original code
	problemInstance = ::newProblem(problemSize);
	solutionInstance = ::allocSolution(problemInstance);
	::randomSolution(solutionInstance);
	solutionInstance2 = ::allocSolution(problemInstance);
	::randomSolution(solutionInstance2);
	moveInstance = ::allocMove(problemInstance);
	psInstance = ::allocPathState(problemInstance);

	return true;
}


void NQueensEvalOp::populateSolutionInstance(IndividualP ind, struct solution* s)
{
	// get Permutation genotype from the individual
	Permutation::Permutation* perm = (Permutation::Permutation*) ind->getGenotype().get();
        
	// populate solution instance
	for(uint i = 0; i < perm->getSize(); i++)
		s->data[i] = perm->variables[i];
}


void NQueensEvalOp::readSolutionInstance(IndividualP ind, struct solution* s)
{
	// get Permutation genotype from the individual
	Permutation::Permutation* perm = (Permutation::Permutation*) ind->getGenotype().get();
        
	// read solution instance
	for(uint i = 0; i < perm->getSize(); i++)
		perm->variables[i] = s->data[i];
}


MoveP NQueensEvalOp::randomMove(IndividualP ind)
{
	populateSolutionInstance(ind, solutionInstance);
	NQueensMove* m (new NQueensMove(moveInstance));
	::randomMove(m->v, solutionInstance);
	return (MoveP) m;
}


bool NQueensEvalOp::applyMove(IndividualP& ind, MoveP move)
{
	NQueensMove* m = (NQueensMove*) move.get();
	populateSolutionInstance(ind, solutionInstance);
	//::printMove(m->v);
	//::printSolution(solutionInstance);
	::applyMove(solutionInstance, m->v);
	//::printSolution(solutionInstance);
	readSolutionInstance(ind, solutionInstance);
	return true;
}


PathP NQueensEvalOp::initPathTo(IndividualP ind1, IndividualP ind2)
{
	populateSolutionInstance(ind1, solutionInstance);
	populateSolutionInstance(ind2, solutionInstance2);
	::initPathTo(psInstance, solutionInstance, solutionInstance2);
	NQueensPath* path (new NQueensPath(psInstance));
    return (PathP) path; 
}


int NQueensEvalOp::getPathLength(PathP path)
{
	NQueensPath* p = (NQueensPath*) path.get();
	return ::getPathLength(p->ps);
}


MoveP NQueensEvalOp::nextRandomMove(PathP path)
{
	NQueensMove* m (new NQueensMove(moveInstance));
	::nextRandomMove(m->v, psInstance);
	return (MoveP) m;
}


FitnessP NQueensEvalOp::evaluate(IndividualP individual)
{
    // minimization
    FitnessP fitness (new FitnessMin);

	populateSolutionInstance(individual, solutionInstance);

	// evaluate
	struct solution* s = solutionInstance;
	int i, j, n = s->n, att, nmod = s->nmod;
	att = 0;
	for (i = 0; i < n-1; i++)
		for (j = i+1; j < n; j++)
			att += (j - i == abs(s->data[i] - s->data[j]));
	s->objvalue = att;
	memcpy(s->olddata, s->data, n * sizeof (int));
	s->nmod = 0;

    fitness->setValue(s->objvalue);

    return fitness;
}
