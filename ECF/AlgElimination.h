#ifndef Elimination_h
#define Elimination_h

#include "Algorithm.h"
#include "SelFitnessProportionalOp.h"
#include "SelRandomOp.h"
#include "SelBestOp.h"

/**
 * \brief Elimination (generation gap) algorithm with roulette wheel elimination selection operator
 * \ingroup algorithms serial
 *
 * This algorithm is genotype independent (it can be used with any Genotype).
 *
 * The algorithm flow:
 *
 \verbatim
  single generation {
   eliminate (genGap * deme_size) individuals from current generation (inverse fitness proportional selection operator);
   create new individuals with crossover (random selection);
   perform mutation on new generation;
  }
 \endverbatim
 */
class Elimination : public Algorithm
{
public:
	Elimination();
	bool advanceGeneration(StateP state, DemeP deme);
	bool initialize(StateP state);
	void registerParameters(StateP state);

protected:
	double genGap_;			///< generation gap
	double selPressure_;	///< selection pressure
	SelFitnessProportionalOpP selFitPropOp;
	SelRandomOpP selRandomOp;
	SelBestOpP selBestOp;
};
typedef boost::shared_ptr<Elimination> EliminationP;

#endif // Elimination_h