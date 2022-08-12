#ifndef Genetic_Annealing_h
#define Genetic_Annealing_h

#include "Algorithm.h"


/**
 * \brief Genetic annealing algorithm (see e.g. http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.56.7606, 
 * http://drdobbs.com/architecture-and-design/184409333?pgno=10)
 * \ingroup algorithms serial
 *
 * Currently implemented only for minimization problems!
 *
 * This algorithm is genotype independent (it can be used with any Genotype).
 *
 */
class GeneticAnnealing : public Algorithm
{
public:
	GeneticAnnealing();

	bool advanceGeneration(StateP state, DemeP deme);
	bool initialize(StateP state);
	void registerParameters(StateP state);

protected:
	bool isFirstGeneration_;
	double energyBank_;
	double *Th;               //threshold 
	double coolingF_;         //factor of cooling 0<coolingF<1
	double dE_;               //reheating increment
	SelBestOpP selBestOp_;
	bool elitism_;

};
typedef boost::shared_ptr<GeneticAnnealing> GeneticAnnealingP;

#endif // Genetic_Annealing_h

