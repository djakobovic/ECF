#ifndef BINARYCRSNONGEOMETRIC_H_
#define BINARYCRSNONGEOMETRIC_H_

#include "Binary.h"

namespace Binary
{


/**
 * \brief Binary genotype: non geometric crossover operator
 */
class BinaryCrsNonGeometric: public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<BinaryCrsNonGeometric> BinaryCrsNonGeometricP;
}

#endif /* BINARYCRSNONGEOMETRIC_H_ */

