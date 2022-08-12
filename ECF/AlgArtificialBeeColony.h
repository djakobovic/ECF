#ifndef Artificial_Bee_Colony_h
#define Artificial_Bee_Colony_h


#include "Algorithm.h"
#include "floatingpoint/FloatingPoint.h"


/**
 * \brief Artificial Bee Colony algorithm (see e.g. http://www.scholarpedia.org/article/Artificial_bee_colony_algorithm)
 * \ingroup algorithms serial
 * 
 * ABC algorithm accepts only a single FloatingPoint genotype (vector of real values).
 * Additionally, it adds the following genotype for algorithm implementation:
 * - FloatingPoint genotype as trial (a cycle counter for each individual)
 */
class ArtificialBeeColony : public Algorithm
{
protected:
    // declare all available selection operators (not all get used)
    SelRandomOpP selRandomOp;
	SelBestOpP selBestOp;
	SelWorstOpP selWorstOp;
	SelFitnessProportionalOpP selFitOp;
    
    uint limit_;
	bool elitism_;
	double ubound_;
	double lbound_;
	std::vector< double > probability_;
	bool isTrialAdded_;

	bool employedBeesPhase(StateP state, DemeP deme);
	bool onlookerBeesPhase(StateP state, DemeP deme);
	bool calculateProbabilities(StateP state, DemeP deme);
	bool scoutBeesPhase(StateP state, DemeP deme);
	bool createNewFoodSource(IndividualP food, StateP state, DemeP deme);

public:
	ArtificialBeeColony();
	void registerParameters(StateP state);
	bool initialize(StateP state);
	bool advanceGeneration(StateP state, DemeP deme);


};
typedef boost::shared_ptr<ArtificialBeeColony> ArtificialBeeColonyP;

#endif // Artificial_Bee_Colony_h

