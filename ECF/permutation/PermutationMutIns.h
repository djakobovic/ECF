#ifndef PERMUTATIONMUTINS_H_
#define PERMUTATIONMUTINS_H_


namespace Permutation
{

/**
 * \ingroup genotypes permutation
 * \brief Permutation genotype: insert mutation operator (see e.g. http://dx.doi.org/10.1016/j.amc.2007.10.013)
 */
class PermutationMutIns: public MutationOp {
public:
	bool mutate(GenotypeP gene);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<PermutationMutIns> PermutationMutInsP;
}
#endif /* PERMUTATIONMUTINS_H_ */