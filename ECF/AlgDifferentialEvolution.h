#ifndef DifferentialEvolution_h
#define DifferentialEvolution_h
using namespace std;
#include "Algorithm.h"

/**
 * \brief Differential evolution (DE) optimization algorithm (see e.g. http://en.wikipedia.org/wiki/Differential_evolution)
 * \ingroup algorithms serial
 *
 *  DE algorithm accepts only a single FloatingPoint genotype (vector of real values).
 */
class DifferentialEvolution : public Algorithm
{
public:
	DifferentialEvolution();
	bool advanceGeneration(StateP state, DemeP deme);
	bool initialize(StateP state);
	void createDonorVectors(DemeP deme, StateP state);
	void crossover(DemeP deme, uint index, StateP state);
	void registerParameters(StateP state);

protected:
	double Fconst_;	//!< scaling constant
	double CR_;	//!< crossover rate
	bool bounded_; //!< constrained or not
	SelectionOperatorP selRandomOp;
	std::vector<IndividualP> donor_vector;	//!< vector of donor solutions (individuals)
};
typedef boost::shared_ptr<DifferentialEvolution> DifferentialEvolutionP;

#endif // Differential Evolution 

