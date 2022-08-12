#ifndef BINARYRANDOMRESPECTFULCRS_H_
#define BINARYRANDOMRESPECTFULCRS_H_

#include "Binary.h"

namespace Binary
{

/**
 * \brief Binary genotype: random respectful crossover operator. Described on http://www.tomaszgwiazda.com/RandomRX.htm. Evolve only one child instead of two as described on-line
 */
class BinaryCrsRandomRespectful: public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<BinaryCrsRandomRespectful> BinaryCrsRandomRespectfulP;
}

#endif /* BINARYRANDOMRESPECTFULCRS_H_ */