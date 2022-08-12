#ifndef AlgSGenGPEA_h
#define AlgSGenGPEA_h

#include "ParallelAlgorithm.h"

/**
 * \ingroup algorithms paralg
 * \brief Synchronous generational global parallel algorithm.
 */
class AlgSGenGpea : public ParallelAlgorithm
{
public:
	AlgSGenGpea();
	bool advanceGeneration(StateP state, DemeP deme);
	bool initialize(StateP state);
	void registerParameters(StateP state);

protected:
	uint jobSize;	// no. of individuals in a single job
	std::vector<IndividualP> myJob;

	double crxRate;
	double selPressure;	// selection pressure
	SelFitnessProportionalOpP selFitPropOp;
	SelRandomOpP selRandomOp;
	SelBestOpP selBestOp;
};
typedef boost::shared_ptr<AlgSGenGpea> AlgSGenGpeaP;

#endif // AlgSGenGPEA_h