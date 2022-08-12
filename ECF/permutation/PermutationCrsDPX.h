#ifndef PERMUTATIONCRSDPX_H
#define PERMUTATIONCRSDPX_H


namespace Permutation
{

/**
 * \ingroup genotypes permutation
 * \brief Permutation genotype: DPX crossover operator (see e.g. itc.ktu.lt/itc342/Misev342.pdf)
 */
class PermutationCrsDPX : public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);	
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<PermutationCrsDPX> PermutationCrsDPXP;
}
#endif //PERMUTATIONCRSDPX_H
