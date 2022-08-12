#ifndef State_h
#define State_h

class Algorithm;
typedef boost::shared_ptr<Algorithm> AlgorithmP;
class State;
typedef boost::shared_ptr<State> StateP;

#include "Population.h"
#include "Randomizer.h"
#include "Logger.h"
#include "EvaluateOp.h"
#include "HallOfFame.h"
#include "StatCalc.h"
#include "Operator.h"
#include "Communicator.h"
#include "Migration.h"
#include "Context.h"

#include <map>
#include "boost/enable_shared_from_this.hpp"

// XML configuration nodes
#define NODE_REGISTRY   "Registry"
#define NODE_ALGORITHM  "Algorithm"
#define NODE_GENOTYPE   "Genotype"
#define NODE_MILESTONE  "Milestone"
#define NODE_POPULATION "Population"


/**
 * \ingroup evol main
 * \brief State class - backbone of the framework
 *
 * State class handles user communication, component repository and evolution process. 
 * State is also the entry point for access to system parameters and functionality.
 */
class State : public boost::enable_shared_from_this<State>
{
private:
	// system components
	RandomizerP randomizer_;
	RegistryP registry_;
	LoggerP logger_;
	CommunicatorP comm_;
	MigrationP migration_;

	// evolutionary components
	PopulationP population_;                //!< sptr to the population
	AlgorithmP algorithm_;                  //!< sptr to evolutionary algorithm used
	CrossoverP crossover_;                  //!< sptr to container of crossover operators
	MutationP mutation_;                    //!< sptr to container of mutation operators
	EvolutionContextP context_;             //!< sptr to evolutionary context
	EvaluateOpP evalOp_;                    //!< sptr to evaluator class
	FitnessP fitness_;                      //!< sptr to fitness object
	IndividualP individual_;                //!< sptr to individual object
	std::vector<GenotypeP> genotype_;       //!< vector of sptr's to genotypes used in individuals

	std::map<std::string, AlgorithmP> mAlgorithms_;	//!< existing algorithms
	typedef std::map<std::string, AlgorithmP>::iterator alg_iter;
	std::map<std::string, GenotypeP> mGenotypes_;	//!< existing genotypes
	typedef std::map<std::string, GenotypeP>::iterator gen_iter;
	std::vector<OperatorP> allTerminationOps_;	//!< existing termination operators
	std::vector<OperatorP> activeTerminationOps_;	//!< active (used) termination ops
	std::vector<OperatorP> allUserOps_;	//!< existing user operators
	std::vector<OperatorP> activeUserOps_;	//!< active (used) user ops
	XMLNode xConfig_;	//!< configuration file data

	// system state
	bool bInitialized_;                     //! is everything ready for evolution
	bool bCommandLine_;                     //! did user issue some command line option
	bool bSaveMilestone_;                   //! should we save milestone
	bool bLoadMilestone_;                   //! should milestone be loaded from given file
	bool bAlgorithmSet_;                    //! is algorithm set
	bool bGenotypeSet_;                     //! is at least one genotype set
	bool bEvaluatorSet_;                    //! is evaluator class set
	bool bBatchMode_;                       //! running in batch mode
	bool bBatchStart_;                      //! batch mode active but not yet initialized
	bool bBatchSingleMilestone_;            //! use single milestone for all batch runs
	bool bBatchWriteStats_;                 //! should batch stats be written to file
	uint batchRepeats_;
	uint batchRemaining_;
	std::string batchStatsFile_;
	std::string batchLogFile_;
	std::string milestoneFilename_;
	uint milestoneInterval_;
	uint milestoneGeneration_;
	time_t milestoneElapsedTime_;
	time_t startTime_, currentTime_, elapsedTime_;
	int argc_;
	char **argv_;

	StateP state_;	// pointer to itself (to avoid http://www.boost.org/doc/libs/1_37_0/libs/smart_ptr/sp_techniques.html#from_this)
	virtual StateP getState()
	{	return shared_from_this();	}

	bool initializeComponents(int, char**);
	bool runBatch();
	bool parseCommandLine(int, char**);
	bool parseConfig(std::string filename);
	bool parseAlgorithmNode(XMLNode node);
	bool parseGenotypeNode(XMLNode node);
	void registerParameters(void);
	void readParameters(void);
	void saveMilestone(void);
	void loadMilestone(void);
	void write(XMLNode&);
	void dumpParameters(std::string fileName, bool addClear = true);


public:
	State();
	virtual ~State()
	{	}
	bool initialize(int, char**);
	bool run();
	FitnessP getFitnessObject();

	/// get current Randomizer
	RandomizerP getRandomizer()	
	{	return randomizer_;	}

	/// set Randomizer to be used
	void setRandomizer(RandomizerP randomizer)	
	{	randomizer_ = randomizer;	}

	/// get access to parameter repository
	RegistryP getRegistry() 
	{	return registry_;	}

	/// get Logger
	LoggerP getLogger() 
	{	return logger_;	}

	/// get population statistics
	StatCalcP getStats()	
	{	return population_->getStats();	}

	/// get population hall of fame
	HallOfFameP getHoF()	
	{	return population_->getHof();	}

	/// access current process communicator mechanism
	CommunicatorP getCommunicator()	
	{	return comm_;	}

	/// get current generation number
	uint getGenerationNo()	
	{	return context_->generationNo_;	}

	/// get elapsed time (in seconds)
	uint getElapsedTime()
	{	return (uint) elapsedTime_;	}

	/// get batch mode info (yes or no)
	bool getBatchMode()
	{	return bBatchMode_;	}

	/// set termination condition (evolution ends with current generation)
	void setTerminateCond() 
	{	context_->bTerminate_ = true;	}

	/// increase and return number of evaluations (local deme)
	uint increaseEvaluations(uint nEval = 1)
	{	return population_->at(0)->stats_->increaseEvaluations(nEval);	}

	/// get number of evaluations (global population)
	uint getEvaluations() 
	{	return population_->getStats()->getEvaluations();	}

	/// is termination condition set
	bool getTerminateCond()	
	{	return context_->bTerminate_;	}

	/// get evolutionary context
	EvolutionContextP getContext()	
	{	return context_;	}

	/// get one initial Individual object
	IndividualP getIndividualObject()
	{	return individual_;	}

	/// access all active (currently used) genotypes
	std::vector<GenotypeP> getGenotypes()
	{	return genotype_;	}

	/// get Population
	PopulationP getPopulation()	
	{	return population_;	}

	/// access current active Algorithm
	AlgorithmP getAlgorithm()
	{	return algorithm_;	}

	/// access evaluation operator
	EvaluateOpP getEvalOp()
	{	return evalOp_;	}

	bool isImplicitParallel();
	bool isAlgorithmParallel();


// TODO: dynamic insertion of framework components
	//addMutationOp   // genotype
	//addCrossoverOp  // genotype

// setting the components (the ones to be used)
	uint setGenotype(GenotypeP);
	void setAlgorithm(AlgorithmP);
	void setEvalOp(EvaluateOpP);
	void setEvalOp(EvaluateOp*);

// adding the components (that _may_ be used or configured)
	bool addGenotype(GenotypeP gen);
	bool addAlgorithm(AlgorithmP alg);
	bool addOperator(OperatorP op);

};
typedef boost::shared_ptr<State> StateP;

#endif // State_h

