#ifndef Roulette_Wheel_h
#define Roulette_Wheel_h

#include "Algorithm.h"
#include "SelRandomOp.h"
#include "SelBestOp.h"
#include "SelFitnessProportionalOp.h"

/**
 * \brief Generational algorithm with roulette wheel selection operator
 * \ingroup algorithms serial
 *
 * This algorithm is genotype independent (it can be used with any Genotype).
 *
 * The algorithm flow:
 *
 \verbatim
  single generation {
   select individuals to form the new generation (fitness proportional selection operator);
   create new generation (make copies);
   noCrx = (deme size) * <crxRate_> / 2;
   repeat(<noCrx> times) {
    randomly select two parents;
    perform crossover, _replace_ parents with their children;
   }
   perform mutation on new generation;
  }
 \endverbatim
 *  
 */
class RouletteWheel : public Algorithm
{
public:
	RouletteWheel();
	bool advanceGeneration(StateP state, DemeP deme);
	bool initialize(StateP state);
	void registerParameters(StateP state);

protected:
	double crxRate_;	//!< crossover rate
	double selPressure_;	//!< selection pressure
	SelFitnessProportionalOpP selFitPropOp;
	SelRandomOpP selRandomOp;
	SelBestOpP selBestOp;

};
typedef boost::shared_ptr<RouletteWheel> RouletteWheelP;

#endif // Roulette_Wheel_h