#ifndef PERMUTATIONCRSSPX_H
#define PERMUTATIONCRSSPX_H


namespace Permutation
{

/**
 * \ingroup genotypes permutation
 * \brief Permutation genotype: SPX crossover operator (see e.g. itc.ktu.lt/itc342/Misev342.pdf)
 */
class PermutationCrsSPX : public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);	
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<PermutationCrsSPX> PermutationCrsSPXP;
}
#endif //PERMUTATIONCRSSPX_H
