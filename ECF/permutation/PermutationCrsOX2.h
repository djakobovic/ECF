#ifndef PERMUTATIONCRSOX2_H
#define PERMUTATIONCRSOX2_H

#include "Permutation.h"


namespace Permutation
{

/**
 * \ingroup genotypes permutation
 * \brief Permutation genotype: Order crossover operator variant where algorithm starts from the beginning when copying the values from second parent (adapted from HeuristicLab)
 */
class PermutationCrsOX2 : public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<PermutationCrsOX2> PermutationCrsOX2P;
}
#endif //PERMUTATIONCRSOX2_H
