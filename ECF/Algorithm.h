#ifndef Algorithm_h
#define Algorithm_h

#include "State.h"
#include "SelectionOperator.h"
#include "Crossover.h"
#include "Mutation.h"
#include "EvaluateOp.h"
#include<vector>

/**
 * \ingroup algorithms serial paralg
 * \brief Algorithm base class
 *
 * All algorithms must inherit this one. 
 * An algorithm combines selection operators (SelectionOperator), crossover (Algorithm::mate),
 * mutation (Algorithm::mutate) and additional logic to build an evolutionary procedure.
 */
class Algorithm
{
protected:
	DemeP activeDeme_;
	std::string name_;                               //!< algorithm name
	bool bImplicitParallel_;                         //!< implicit parallel flag 
	std::vector<SelectionOperatorP> selectionOp;	 //!< sel. operators used by algorithm

	/**
	 * \brief Helper function: register a single parameter with the system 
	 * \param	state	pointer to State object
	 * \param	name	unique parameter name
	 * \param	value	pointer to parameter value object (caller provides)
	 * \param	T	parameter type (see Registry.h)
	 * \return	true if parameter successfuly registered, false otherwise
	 */
	bool registerParameter(StateP state, std::string name, voidP value, enum ECF::type T, std::string description = "")
	{
		return state->getRegistry()->registerEntry(name_ + "." + name, value, T, description);
	}

	/**
	 * \brief Helper function: get parameter value from the system 
	 * \param	state	pointer to State object
	 * \param	name	parameter name
	 * \return	pointer to parameter value (needs casting)
	 */
	voidP getParameterValue(StateP state, std::string name)
	{
		return state->getRegistry()->getEntry(name_ + "." + name);
	}

#ifdef _MPI
	std::vector<IndividualP> requests_, stored_;           //!< individual vectors for implicit evaluation
	std::vector<uint> requestIds_;                         //!< individual indexes for implicit evaluation
	std::vector<IndividualP> demeCopy_;
	std::vector<IndividualP> myJob_;                       //!< worker's individual vector
	CommunicatorP comm_;
	uint jobSize_;
	uint totalEvaluations_, wastedEvaluations_;
	bool bImplicitEvaluation_;                             //!< implicit evaluation flag
	bool bImplicitMutation_;                               //!< implicit mutation flag
	bool bSynchronized_;                                   //!< is implicit paralelization synchronous
	SelectionOperatorP selBestOp;
	std::vector<IndividualP> requestsMut_, receivedMut_;   //!< individual vectors for implicit mutation
	std::vector<uint> requestMutIds_;                      //!< individual indexes for implicit mutation
	IndividualP currentBest_;

	std::vector<IndividualP> storedInds_;                  //<! last consistent version of an individual
	std::vector< std::vector<IndividualP> > sentInds_;     //!< individuals sent for evaluation
	std::vector<bool> isConsistent_;                       //!< is individual (genotype-fitness pair) consistent

	void storeIndividual(IndividualP);                     //!< stores the individual (if it is consistent), resets consistency flag
	void storeGenotypes(std::vector<IndividualP>&);        //!< adds genotypes of individuals to 'sent' repository
	void setConsistency(IndividualP);                      //!< denotes current individual as consistent
	void restoreIndividuals(std::vector<uint>);            //!< restores individuals whose fitness is received
	void restorePopulation();                              //!< restores inconsistent individuals to last consistent state
#endif


public:
	CrossoverP crossover_;                                 //!< sptr to container of crossover operators (set by the system)
	MutationP mutation_;                                   //!< sptr to container of mutation operators (set by the system)
	EvaluateOpP evalOp_;                                   //!< sptr to evaluation operator (set by the system)
	StateP state_;

	Algorithm()
	{	bImplicitParallel_ = false;	}

	virtual ~Algorithm()
	{	}

	//! Get algorithm name. Each algorithm is uniquely identified with its name.
	std::string getName()
	{	return name_;	}

	//! Is algorithm parallel (false by default for all algorithms not inheriting ParallelAlgorithm class).
	virtual bool isParallel()
	{	return false;	}

	//! Is algorithm run in implicit parallel mode (in development, see tutorial).
	bool isImplicitParallel()
	{	return bImplicitParallel_;	}

	/**
	 * \brief Perform a single generation on a single deme.
	 *
	 * Must be implemented by a specific algorithm class
	 */
	virtual bool advanceGeneration(StateP, DemeP) = 0;

	/**
	 * \brief Initialize the algorithm, read parameters from the system, do a sanity check.
	 *
	 * Called by the system before the algorithm starts (Algorithm::advanceGeneration)
	 */
	virtual bool initialize(StateP)
	{	return true;	}

	/**
	 * \brief Register algorithm's parameters (if any).
	 *
	 * Called by the system before algorithm initialization (Algorithm::initialize)
	 */
	virtual void registerParameters(StateP)	{}

	/**
	 * \brief Evaluate initial population (called by State::run before evolution starts).
	 */
	virtual bool initializePopulation(StateP);

	virtual void read()	{}

#ifndef _MPI

	/**
	 * \brief Perform one generation of the algorithm on the whole population.
	 * 
	 * Default if not reimplemented: algorithm advances one generation on each deme
	 */
	virtual bool advanceGeneration(StateP state)
	{
		ECF_LOG(state, 4, name_ + ": starting generation");

		bool bResult = true;
		for(uint iDeme = 0; iDeme < state->getPopulation()->size(); iDeme++) {
			activeDeme_ = state->getPopulation()->at(iDeme);
			bResult |= advanceGeneration(state, state->getPopulation()->at(iDeme));
		}

		ECF_LOG(state, 4, name_ + ": ending generation");

		return bResult;
	}

	/**
	 * \brief Helper function: evaluate an individual 
	 */
	void evaluate(IndividualP ind)
	{
		state_->getContext()->evaluatedIndividual = ind;
		ECF_LOG(state_, 5, "Evaluating individual: " + ind->toString());
		ind->fitness = evalOp_->evaluate(ind);
		ECF_LOG(state_, 5, "New fitness: " + ind->fitness->toString());
		state_->increaseEvaluations();
	}

	/**
	 * \brief Helper function: send a vector of individuals to mutation
	 */
	uint mutate(const std::vector<IndividualP>& pool)
	{	return mutation_->mutation(pool);	}

	/**
	 * \brief Helper function: send a single individual to mutation
	 */
	uint mutate(const IndividualP victim)
	{	
		std::vector<IndividualP> pool;
		pool.push_back(victim);
		return mutation_->mutation(pool);
	}

	/**
	 * \brief Helper function: replace an individual in current deme
	 * @param oldInd	individual to be replaced
	 * @param newInd	new individual
	 */
	void replaceWith(IndividualP oldInd, IndividualP newInd)
	{	activeDeme_->replace(oldInd->index, newInd);	}

	/**
	 * \brief Helper function: replace an individual at given position in current deme
	 * @param oldIndId	index of individual to be replaced
	 * @param newInd	new individual
	 */
	void replaceWith(uint oldIndId, IndividualP newInd)
	{	activeDeme_->replace(oldIndId, newInd);	}

	/// used only in parallel ECF
	void registerParallelParameters(StateP state)
	{	}

	/// used only in parallel ECF
	bool initializeParallel(StateP state)
	{	return true;	}

#else	// implicit parallel version

	/// Parallel ECF: Perform one generation of the algorithm.
	virtual bool advanceGeneration(StateP state);

	/// Parallel ECF: broadcast termination to worker processes.
	virtual void bcastTermination(StateP state);

	/// Parallel ECF: Register parameters for implicit parallel algorithm version.
	void registerParallelParameters(StateP state);

	/// Parallel ECF: Initialize implicit parallel algorithm.
	bool initializeParallel(StateP state);

	/// Parallel ECF: Initialize implicit parallel mode
	void initializeImplicit(StateP state);

	///Parallel ECF: Worker processes in implicit parallel algorithm.
	bool implicitParallelOperate(StateP state);

	/// Parallel ECF: evaluate an individual.
	void evaluate(IndividualP ind)
	{
		// implicit evaluation
		if(bImplicitEvaluation_)
			implicitEvaluate(ind);
		// if implicit mutation is active, check is evaluation needed
		else if(bImplicitMutation_ && ind->fitness->isValid())
			return;
		// else, evaluate the individual
		state_->getContext()->evaluatedIndividual = ind;
		ECF_LOG(state_, 5, "Evaluating individual: " + ind->toString());
		ind->fitness = evalOp_->evaluate(ind);
		ECF_LOG(state_, 5, "New fitness: " + ind->fitness->toString());
		state_->increaseEvaluations();
	}

	/// Parallel ECF: implicitly evaluate an individual (store for later evaluation in implicit parallel version).
	void implicitEvaluate(IndividualP ind);

	/// Parallel ECF: immediately mutate an individual.
	uint mutate(const IndividualP victim)
	{
		std::vector<IndividualP> pool;
		pool.push_back(victim);
		return mutate(pool);
	}

	/// Parallel ECF: mutate a vector of individuals.
	uint mutate(const std::vector<IndividualP>& pool)
	{
		if(bImplicitMutation_) {
			uint nMutations = 0;
			for(uint i = 0; i < pool.size(); i++) {
				nMutations += implicitMutate(pool[i]);
			}
			return nMutations;
		}
		else
			return mutation_->mutation(pool);
	}

	/// Parallel ECF: implicitly mutate an individual (store for later mutation in implicit parallel version).
	uint implicitMutate(IndividualP ind);

	/// Parallel ECF: replace an individual in a deme.
	void replaceWith(IndividualP oldInd, IndividualP newInd);

	/// Parallel ECF: replace an individual at given position in a deme.
	void replaceWith(uint oldIndId, IndividualP newInd)
	{	replaceWith(state_->getPopulation()->getLocalDeme()->at(oldIndId), newInd);	}

#endif

// common functions (serial and parallel)

	/**
	 * \brief Helper function: crossover two individuals
	 */
	bool mate(IndividualP p1, IndividualP p2, IndividualP child)
	{	return crossover_->mate(p1, p2, child);	}

	/**
	 * \brief Helper function: make a copy of an individual
	 */
	IndividualP copy(IndividualP source)
	{	return (IndividualP) source->copy();	}

	/**
	 * \brief Helper function: remove victim from pool of individual pointers
	 */
	bool removeFrom(IndividualP victim, std::vector<IndividualP> &pool)
	{
		uint iWorst = 0;
		while(iWorst < pool.size() && pool[iWorst] != victim)
			iWorst++;
		if(iWorst == pool.size())
			return false;
		pool.erase(pool.begin() + iWorst);
		return true;
	}

	/**
	 * \brief Helper function: check if individual is in the pool
	 */
	bool isMember(IndividualP single, std::vector<IndividualP> &pool)
	{
		uint index = 0;
		while(index < pool.size() && pool[index] != single)
			index++;
		if(index == pool.size())
			return false;
		return true;
	}
};
typedef boost::shared_ptr<Algorithm> AlgorithmP;

#endif // Algorithm_h

