#ifndef PERMUTATIONCRSCYCLIC2_H
#define PERMUTATIONCRSCYCLIC2_H

#include "Permutation.h"


namespace Permutation
{

/**
 * \ingroup genotypes permutation
 * \brief Permutation genotype: Cyclic version 2 crossover operator (adapted from HeuristicLab)
 */
class PermutationCrsCyclic2 : public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);	
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<PermutationCrsCyclic2> PermutationCrsCyclic2P;
}
#endif //PERMUTATIONCRSCYCLIC2_H
