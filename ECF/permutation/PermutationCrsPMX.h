#ifndef PERMUTATIONCRSPMX_H
#define PERMUTATIONCRSPMX_H


namespace Permutation
{

/**
 * \ingroup genotypes permutation
 * \brief Permutation genotype: PMX crossover operator (see e.g. http://dx.doi.org/10.1016/j.amc.2007.10.013)
 */
class PermutationCrsPMX : public CrossoverOp
{
public:

	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);	
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<PermutationCrsPMX> PermutationCrsPMXP;
}
#endif //PERMUTATIONCRSPMX_H

