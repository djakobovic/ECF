#ifndef CuckooSearch_h
#define CuckooSearch_h
#include "Algorithm.h"

/**
 * \brief Cuckoo search (CS) optimization algorithm (see http://en.wikipedia.org/wiki/Cuckoo_search)
 * \ingroup algorithms serial
 * 
 * CS algorithm accepts only FloatingPoint genotype (vector of real values).
 * This implementation is based on: http://www.mathworks.com/matlabcentral/fileexchange/29809-cuckoo-search--cs--algorithm
 *
 * The algorithm flow:
 *
 \verbatim 
  single generation {
   generate new solutions via Levy flights ( adding constants multiplied with normal distribution random numbers and difference between current solution and best solution)
   pa amount of new generated solutions are replaced with new solutions, related to the difference in solutions
   }
 \endverbatim
 *
 */
class CuckooSearch : public Algorithm
{
public:
	CuckooSearch();
	bool advanceGeneration(StateP state, DemeP deme);
	bool initialize(StateP state);
	void registerParameters(StateP state);
protected:
	double pa;	//!< percentage of solutions which will be replaced (similar to mutation probability)
	double ubound;
	double lbound;
	uint numDimension;
	SelectionOperatorP selBestOp;
};

typedef boost::shared_ptr<CuckooSearch> CuckooSearchP;

#endif