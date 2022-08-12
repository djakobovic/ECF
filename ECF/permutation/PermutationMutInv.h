#ifndef PERMUTATIONMUTINV_H_
#define PERMUTATIONMUTINV_H_


namespace Permutation
{

/**
 * \ingroup genotypes permutation
 * \brief Permutation genotype: inversion mutation operator (see e.g. http://dx.doi.org/10.1016/j.amc.2007.10.013)
 */
class PermutationMutInv: public MutationOp {
public:
	bool mutate(GenotypeP gene);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<PermutationMutInv> PermutationMutInvP;
}
#endif /* PERMUTATIONMUTINV_H_ */