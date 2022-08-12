#ifndef AlgAEliGPEA_h
#define AlgAEliGPEA_h

#include "ParallelAlgorithm.h"


/**
 * \ingroup algorithms paralg
 * \brief Asynchronous elimination global parallel algorithm
 */
class AlgAEliGpea : public ParallelAlgorithm
{
public:
	AlgAEliGpea();
	bool advanceGeneration(StateP state, DemeP deme);
	bool initialize(StateP state);
	void registerParameters(StateP state);
	void bcastTermination(StateP);

protected:
	uint jobSize_;	// no. of individuals in a single job
	std::vector<IndividualP> myJob_;
	uint nTournament_;	// tournament size

	IndividualP performSingleTournament(DemeP deme);
};
typedef boost::shared_ptr<AlgAEliGpea> AlgAEliGpeaP;

#endif // AlgAEliGPEA_h
