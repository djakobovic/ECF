#ifndef PERMUTATIONCRSOBX_H
#define PERMUTATIONCRSOBX_H

#include "Permutation.h"


namespace Permutation
{

/**
 * \ingroup genotypes permutation
 * \brief Permutation genotype: Order based crossover operator (see e.g. http://dx.doi.org/10.1016/j.amc.2007.10.013)
 */
class PermutationCrsOBX : public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);	
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<PermutationCrsOBX> PermutationCrsOBXP;
}
#endif //PERMUTATIONCRSOBX_H
