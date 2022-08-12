#ifndef TreeMutPermutation_h
#define TreeMutPermutation_h


namespace Tree
{

/**
 * \ingroup genotypes tree
 * \brief Tree genotype: permutation mutation operator
 */
class TreeMutPermutation : public MutationOp
{
public:
	bool mutate(GenotypeP gene);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<TreeMutPermutation> TreeMutPermutationP;
}
#endif // TreeMutPermutation_h
