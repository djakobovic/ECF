#ifndef PERMUTATIONMUTTOGGLE_H_
#define PERMUTATIONMUTTOGGLE_H_


namespace Permutation
{

/**
 * \ingroup genotypes permutation
 * \brief Permutation genotype: toggle mutation operator (see e.g. http://dx.doi.org/10.1016/j.amc.2007.10.013)
 */
class PermutationMutToggle: public MutationOp {
public:
	bool mutate(GenotypeP gene);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<PermutationMutToggle> PermutationMutToggleP;
}
#endif /* PERMUTATIONMUTTOGGLE_H_ */