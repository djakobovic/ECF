#ifndef PERMUTATIONCRSOPX_H
#define PERMUTATIONCRSOPX_H


namespace Permutation
{

/**
 * \ingroup genotypes permutation
 * \brief Permutation genotype: OPX crossover operator (see e.g. itc.ktu.lt/itc342/Misev342.pdf)
 */
class PermutationCrsOPX : public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);	
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<PermutationCrsOPX> PermutationCrsOPXP;
}
#endif //PERMUTATIONCRS_H
