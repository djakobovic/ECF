#ifndef PERMUTATIONCRSULX_H
#define PERMUTATIONCRSULX_H

#include "Permutation.h"


namespace Permutation
{

/**
 * \ingroup genotypes permutation
 * \brief Permutation genotype: Uniform like crossover operator (see e.g. itc.ktu.lt/itc342/Misev342.pdf)
 */
class PermutationCrsULX : public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<PermutationCrsULX> PermutationCrsULXP;
}
#endif //PERMUTATIONCRSULX_H
