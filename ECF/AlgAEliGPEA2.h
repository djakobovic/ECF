#ifndef AlgAEliGPEA2_h
#define AlgAEliGPEA2_h

#include "ParallelAlgorithm.h"

/**
 * \brief Asynchronous elimination global parallel algorithm (outdated version)
 */
class AlgAEliGpea2 : public Algorithm
{
public:
	AlgAEliGpea2();
	bool advanceGeneration(StateP state, DemeP deme);
	bool initialize(StateP state);
	void registerParameters(StateP state);

protected:
	uint jobSize;	// no. of individuals in a single job
	std::vector<IndividualP> myJob;
	uint nTournament;	// tournament size
};
typedef boost::shared_ptr<AlgAEliGpea2> AlgAEliGpea2P;

#endif // AlgAEliGPEA2_h
