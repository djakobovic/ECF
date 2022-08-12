#ifndef TreeMutSubtree_h
#define TreeMutSubtree_h


namespace Tree
{

/**
 * \ingroup genotypes tree
 * \brief Tree genotype: subtree size-fair mutation operator.
 * This is a 'standard' GP subtree mutation.
 */
class TreeMutSubtree : public MutationOp
{
public:
	bool mutate(GenotypeP gene);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<TreeMutSubtree> TreeMutSubtreeP;
}
#endif // TreeMutSubtree_h
