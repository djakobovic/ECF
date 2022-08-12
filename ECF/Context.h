#ifndef Context_h
#define Context_h

/**
 * \ingroup evol main
 * \brief Evolutionary context class
 *
 * Stores information of termination condition, number of generations and evaluations,
 * individuals being mutated, crossed and evaluated.
 */
class EvolutionContext
{
	friend class State;

public:
	IndividualP evaluatedIndividual;                //!< pointer to Individual being evaluated
	IndividualP mutatedIndividual;                  //!< pointer to Individual being mutated
	IndividualP firstParent, secondParent, child;   //!< pointer to Individuals in current crossover
	void* environment;                              //!< pointer to user defined environment

	/**
	 * initialize evolutionary context
	 */
	bool initialize()
	{
		bTerminate_ = false;
		generationNo_ = 0;
		return true;
	}

protected:
	bool bTerminate_;
	uint generationNo_;
};
typedef boost::shared_ptr<EvolutionContext> EvolutionContextP;

#endif // Context_h
