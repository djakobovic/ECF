#ifndef ParallelAlgorithm_h
#define ParallelAlgorithm_h

#include "Algorithm.h"
#include<vector>

/**
 * \brief Parallel algorithm base class.
 * \ingroup algorithms paralg
 *
 * All parallel algorithms should inherit this one. 
 */
class ParallelAlgorithm : public Algorithm
{
public:
	virtual ~ParallelAlgorithm()
	{	}

	/// Is algorithm parallel (true by default for all algorithms that inherit ParallelAlgorithm class).
	bool isParallel()
	{	return true;	}
};
typedef boost::shared_ptr<ParallelAlgorithm> ParallelAlgorithmP;

#endif // ParallelAlgorithm_h

