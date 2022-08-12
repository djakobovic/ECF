#ifndef Mutation_h
#define Mutation_h

class Individual;
typedef boost::shared_ptr<Individual> IndividualP;
class Genotype;
typedef boost::shared_ptr<Genotype>  GenotypeP;

/**
 * \ingroup evol evoop
 * \brief MutationOp base class
 *
 * Each MutationOp class mutates a _genotype_ object.
 * Each Genotype class must define its own set of MutationOp classes.
 */
class MutationOp
{
public:
	MutationOp()
	{	probability_ = 0;	}

	/**
	 * \brief Performs mutation of a genotype object.
	 * The genotype object must be initialized!
	 */
	virtual bool mutate(GenotypeP) = 0;

	/// Initialize mutation operator. Called before first mutation operation.
	virtual bool initialize(StateP)
	{	return true;	}

	/// Register parameters with the system. Called before MutationOp::initialize.
	virtual void registerParameters(StateP)
	{	}

	virtual ~MutationOp()
	{	}

	StateP state_;
	double probability_;    //!< probability of usage of this mutation operator
	GenotypeP myGenotype_;  //!< pointer to the Genotype that defines this MutationOp
};
typedef boost::shared_ptr<MutationOp> MutationOpP;


/**
 * \ingroup evol evoop
 * \brief Mutation class - handles mutation of _individuals_ (as opposed to MutationOp class that mutates genotypes).
 *
 * Mutation class is in fact a repository of every mutation operator (MutationOp) for every active Genotype 
 * (the genotypes used in individuals).
 * Determines which MutationOp will be used for each genotype. (every operator for every genotype has a usage probability)
 */
class Mutation
{
public:
	bool mutate(IndividualP ind);
	uint mutation(const std::vector<IndividualP>&);
	void registerParameters(StateP);
	bool initialize(StateP);
	double getIndMutProb();
	double setIndMutProb(double);

	std::vector< std::vector<MutationOpP> > operators;  //!< vectors of mutation operators for each genotype
	std::vector< std::vector<double> > opProb;          //!< usage probabilities for each MutationOp operator

	IndividualP currentInd;	// samo za ad hoc lokalnu pretragu!

protected:
	StateP state_;
	double indMutProb_;                     //!< mutation probability of an individual
	uint mutateGenotypes_;                  //!< what genotypes (if more than one) to mutate (random, all)
	std::vector<bool> protectedGenotypes_;  //!< protected (non-mutatable) genotypes flags

};
typedef boost::shared_ptr<Mutation> MutationP;

#endif // Mutation_h
