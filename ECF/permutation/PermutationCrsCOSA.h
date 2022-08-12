#ifndef PERMUTATIONCRSCOSA_H
#define PERMUTATIONCRSCOSA_H

#include "Permutation.h"


namespace Permutation
{

/**
 * \ingroup genotypes permutation
 * \brief Permutation genotype: COSA crossover operator (adapted from HeuristicLab)
 */
class PermutationCrsCOSA : public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);	
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<PermutationCrsCOSA> PermutationCrsCOSAP;
}
#endif //PERMUTATIONCRSCOSA_H
