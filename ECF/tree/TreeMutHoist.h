#ifndef TreeMutHoist_h
#define TreeMutHoist_h


namespace Tree
{

/**
 * \ingroup genotypes tree
 * \brief Tree genotype: mutation operator that replaces original tree with a randomly chosen subtree from the original tree.
 */
class TreeMutHoist : public MutationOp
{
public:
	bool mutate(GenotypeP gene);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<TreeMutHoist> TreeMutHoistP;
}
#endif // TreeMutHoist_h
