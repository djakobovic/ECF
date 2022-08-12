#ifndef BINARYMASKEDCRS_H_
#define BINARYMASKEDCRS_H_

#include "Binary.h"

namespace Binary
{

/**
 * \brief Binary genotype: masked crossover operator. Described on http://www.tomaszgwiazda.com/maskedX.htm. Evolve only one child instead of two as described on-line
 */
class BinaryCrsMasked: public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<BinaryCrsMasked> BinaryCrsMaskedP;
}

#endif /* BINARYMASKEDCRS_H_ */