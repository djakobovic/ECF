#ifndef TreeMutShrink_h
#define TreeMutShrink_h


namespace Tree
{

/**
 * \ingroup genotypes tree
 * \brief Tree genotype: mutation operator that shrinks randomly chosen subtree
 */
class TreeMutShrink : public MutationOp
{
public:
	bool mutate(GenotypeP gene);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<TreeMutShrink> TreeMutShrinkP;
}
#endif // TreeMutShrink_h
