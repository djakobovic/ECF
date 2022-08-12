#include <ECF/ECF.h>
#include "KnapsackEvalOp.h"
#include "knapsack.h"


void KnapsackEvalOp::registerParameters(StateP state)
{
	state->getRegistry()->registerEntry("knapsack.infile", (voidP) (new std::string), ECF::STRING);
}


bool KnapsackEvalOp::initialize(StateP state)
{
	state_ = state;

	if(!state->getRegistry()->isModified("knapsack.infile")) {
		state->getLogger()->log(1, "Error: no input file defined! (parameter 'knapsack.infile'");
		return false;
	}

	voidP sptr = state->getRegistry()->getEntry("knapsack.infile"); // get parameter value
	std::string filePath = *((std::string*) sptr.get()); // convert from voidP to user defined type

	// API calls
	problemInstance = newProblem(filePath.c_str());
	solutionInstance = allocSolution(problemInstance);
	randomSolution(solutionInstance);

	// adjust BitString genotype size
	state->getRegistry()->modifyEntry("BitString.size", (voidP) new uint(problemInstance->n));

	// reinitialize population with updated size
	state->getPopulation()->initialize(state);

	return true;
}


MoveP KnapsackEvalOp::randomMove(IndividualP ind)
{
	KnapsackMove* move (new KnapsackMove);
	move->data = state_->getRandomizer()->getRandomInteger(problemInstance->n);
	return (MoveP) move;
}


bool KnapsackEvalOp::applyMove(IndividualP& ind, MoveP move)
{
    struct problem *p = problemInstance;
	KnapsackMove* m = (KnapsackMove*) move.get();
	BitString::BitString* s = (BitString::BitString*) ind->getGenotype().get();
    int i = m->data;
    if (i < 0 || i >= p->n)     /* null move, do nothing */
        return false;
    if (s->bits[i] == 0) {
        s->bits[i] = 1;
    } else {
        s->bits[i] = 0;
    }
	return true;
}


PathP KnapsackEvalOp::initPathTo(IndividualP ind1, IndividualP ind2)
{
	KnapsackPath* path (new KnapsackPath);
	BitString::BitString* s1 = (BitString::BitString*) ind1->getGenotype().get();
	BitString::BitString* s2 = (BitString::BitString*) ind2->getGenotype().get();

    int i, k, n = problemInstance->n;
    /* Indices of the bits in which the solutions differ */
    for (i = 0, k = 0; i < n; i++)
        if (s1->bits[i] != s2->bits[i])
			path->pos.push_back(i);
	path->distance = path->pos.size();
    return (PathP) path; 
}


int KnapsackEvalOp::getPathLength(PathP path)
{
	return ((KnapsackPath*) path.get())->distance;
}


MoveP KnapsackEvalOp::nextRandomMove(PathP path)
{
	KnapsackMove* v (new KnapsackMove);
	KnapsackPath* ps = (KnapsackPath*) path.get();

	int r;
    if (ps->distance == 0) { /* end of path has been reached, no move is possible */
        v->data = -1;        /* null move */
        return (MoveP) v;
    }
    /* Draw an index at random */
    r = state_->getRandomizer()->getRandomInteger(0, --ps->distance);
    v->data = ps->pos[r]; 
    /* Update pathState - remember which moves are still available (and only those) */
    ps->pos[r] = ps->pos[ps->distance];

	return (MoveP) v;
}


FitnessP KnapsackEvalOp::evaluate(IndividualP individual)
{
    // minimization
    FitnessP fitness (new FitnessMin);

    // Each individual is a vector of genotypes (defined in the configuration file).
    // We'll use BitString, and put it as the first and only genotype
	BitString::BitString* bitstr = (BitString::BitString*) individual->getGenotype().get();
	//BitStringP bitstr = boost::static_pointer_cast<BitString::BitString> (individual->getGenotype(0)); // don't need zero for the first one
        
	// evaluate
	struct solution* s = solutionInstance;
	struct problem *p = problemInstance;
	int n = s->n;
	s->profit = 0, s->weight = 0;
	for (int i = 0; i < n; i++)
		if (bitstr->bits[i]) {
			s->profit += p->profit[i];
			s->weight += p->weight[i];
		}
	s->objvalue = s->weight - p->capacity;
	if (s->objvalue <= 0)
		s->objvalue = -s->profit;

    fitness->setValue(s->objvalue);

    return fitness;
}
