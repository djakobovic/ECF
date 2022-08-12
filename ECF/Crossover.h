#ifndef Crossover_h
#define Crossover_h

#include "Individual.h"

class Individual;
typedef boost::shared_ptr<Individual> IndividualP;
class Genotype;
typedef boost::shared_ptr<Genotype>  GenotypeP;

/**
 * \ingroup evol evoop
 * \brief CrossoverOp base class
 *
 * Each CrossoverOp class crosses a pair of _genotypes_.
 * Each Genotype class must define its own set of CrossoverOp classes.
 */
class CrossoverOp
{
public:
	CrossoverOp()
	{	probability_ = 0;	}

	/**
	 * Performs crossover of two genotype objects.
	 * Each genotype object must be initialized!
	 */
	virtual bool mate(GenotypeP, GenotypeP, GenotypeP) = 0;

	/// Initialize crossover operator. Called before first crossover operation.
	virtual bool initialize(StateP)
	{	return true;	}

	/// Register parameters with the system. Called before CrossoverOp::initialize.
	virtual void registerParameters(StateP)
	{	}

	virtual ~CrossoverOp()
	{	}

	StateP state_;
	double probability_;     //!< probability of usage of this crossover operator
	GenotypeP myGenotype_;   //!< pointer to the Genotype that defines this CrossoverOp
};
typedef boost::shared_ptr<CrossoverOp> CrossoverOpP;


/**
 * \ingroup evol evoop
 * \brief Crossover class - handles crossover of _individuals_ (as opposed to CrossoverOp class that crosses genotypes).
 *
 * Crossover class is in fact a repository of every crossover operator (MutationOp) for every active Genotype 
 * (the genotypes used in individuals).
 * Determines which CrossoverOp will be used for each genotype.
 */
class Crossover
{
public:
	bool mate(IndividualP ind1, IndividualP ind2, IndividualP child);
	void registerParameters(StateP);
	bool initialize(StateP);

	std::vector< std::vector<CrossoverOpP> > operators;   //!< vectors of crx operators for each genotype
	std::vector< std::vector<double> > opProb;            //!< usage probabilities for each CrossoverOp operator

protected:
	StateP state_;
	uint crxGenotypes_;                                   //!< what genotypes (if more than one) to cross (random, all)
	std::vector<bool> protectedGenotypes_;                //!< protected (non-crossable) genotypes flags
};
typedef boost::shared_ptr<Crossover> CrossoverP;

#endif // Crossover_h
