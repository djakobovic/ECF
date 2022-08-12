#ifndef Evolution_Strategy_h
#define Evolution_Strategy_h

#include "Algorithm.h"


/**
 * \brief (mu/rho +/, lambda) - Evolution Strategy (ES) algorithm.
 * \ingroup algorithms serial
 * 
 * This algorithm is genotype independent (it can be used with any Genotype).
 * 
 * The algorithm parameters are:
 *		- mu: the size of the parent population
 *		- lambda: number of offspring created in each iteration
 *		- rho: number of parents used to create an offspring; 1: mutation is used, 2: crossover
 *		- selection: may be "plus", which uses both parents and offspring or "comma", which uses just the offspring pool
 *
 * Since the ECF defines "population.size" independently of the algorithm, this implementation actually runs multiple
 * ES parent populations; the population.size must therefore be a multiple of parent pool size (mu). 
 * For instance, if mu = 5 and population.size = 30, then 6 ES subpopulations will be executed (no migration).
 *							
 * The algorithm flow:
 *
 \verbatim
  subpopulations = population.size / mu;
  single generation {
   repeat(for every subpopulation) {
    add mu individuals to the parent pool;
    create lambda offspring using random rho parents for each;
	if comma selection 
	  create new parent pool with mu best from offspring pool;
    else
	  create new parent pool with mu best from offspring and parents pool;
   }
  }
 \endverbatim
 *
 */
class EvolutionStrategy : public Algorithm
{
public:
	EvolutionStrategy();
	bool advanceGeneration(StateP state, DemeP deme);
	bool initialize(StateP state);
	void registerParameters(StateP state);

protected:
	uint lambda_;         //!< number of offspring
	uint rho_;            //!< number of parents (1 or 2)
	uint mu_;             //!< the size of the parent population
	uint subPopulations_; //!< how many parent populations are in a deme
	bool plusSelection_;  //!< type of selection (plus or comma)
	SelectionOperatorP  selBestOp_, selRandomOp_;

	static bool compare(IndividualP first, IndividualP second)
	{	return first->fitness->isBetterThan(second->fitness);	}

};
typedef boost::shared_ptr<EvolutionStrategy> EvolutionStrategyP;

#endif // Evolution_Strategy_h

