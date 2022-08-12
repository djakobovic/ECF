#ifndef PERMUTATIONCRSCYCLIC_H
#define PERMUTATIONCRSCYCLIC_H

#include "Permutation.h"


namespace Permutation
{

/**
 * \ingroup genotypes permutation
 * \brief Permutation genotype: Cyclic crossover operator (see e.g. http://www.rubicite.com/Tutorials/GeneticAlgorithms/CrossoverOperators/CycleCrossoverOperator.aspx)
 */
class PermutationCrsCyclic : public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);	
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<PermutationCrsCyclic> PermutationCrsCyclicP;
}
#endif //PERMUTATIONCRSCYCLIC_H
