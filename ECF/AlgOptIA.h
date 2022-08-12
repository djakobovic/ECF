#ifndef OptIA_h
#define OptIA_h

#include "Algorithm.h"
#include "floatingpoint/FloatingPoint.h"


/**
 *\brief Optimization Immune Algorithm (opt-IA, see e.g. http://www.artificial-immune-systems.org/algorithms.shtml).
 * \ingroup algorithms serial
 * 
 * This opt-IA implements:  
 *			- static cloning: all antibodies are cloned dup times, making the size of the clone population equal dup * poplationSize
 *			- inversely proportional hypermutation: better antibodies are mutated less
 *			- static pure aging - if an antibody exceeds tauB number of trials, it is replaced with a new randomly created antibody
 *			- birthPhase: if the number of antibodies that survive the aging Phase is less than populationSize, new randomly created abs are added to the population
 *			- optional elitism
 *
 * opt-IA algorithm accepts only a single FloatingPoint genotype. 
 * Additionally, opt-IA adds a FloatingPoint genotype (age). 
 */
class OptIA : public Algorithm
{
protected:

	double ubound;
	double lbound;
	uint dimension;
	bool areGenotypesAdded_;

	uint dup;		//!< number of clones per antibody
	double c;		//!< mutation parameter
	double tauB;	//!< maximum number of generations without improvement 
	bool elitism;	//!< specifies whether to use elitism or not

	//! sort vector of antibodies in regards to their fitness
	static bool sortPopulationByFitness (IndividualP ab1,IndividualP ab2) { return ( ab1->fitness->isBetterThan(ab2->fitness)); }

public:

    OptIA();
    void registerParameters(StateP state);
	bool initialize(StateP state);
	bool advanceGeneration(StateP state, DemeP deme);

	bool cloningPhase(StateP state, DemeP deme, std::vector<IndividualP> &clones);
	bool hypermutationPhase(StateP state, DemeP deme, std::vector<IndividualP> &clones);
	bool agingPhase(StateP state, DemeP deme,  std::vector<IndividualP> &clones);
	bool selectionPhase(StateP state, DemeP deme, std::vector<IndividualP> &clones);
	bool birthPhase(StateP state, DemeP deme, std::vector<IndividualP> &clones);
	bool replacePopulation(StateP state, DemeP deme, std::vector<IndividualP> &clones);

};
typedef boost::shared_ptr<OptIA> OptIAP;


#endif // OptIA_h

