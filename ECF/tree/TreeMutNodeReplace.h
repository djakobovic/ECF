#ifndef TreeMutNodeReplace_h
#define TreeMutNodeReplace_h


namespace Tree
{

/**
 * \ingroup genotypes tree
 * \brief Tree genotype: node replacement mutation operator.
 * Tries to replace the selected primitive with a different one with the same number of arguments.
 */
class TreeMutNodeReplace : public MutationOp
{
public:
	bool mutate(GenotypeP gene);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<TreeMutNodeReplace> TreeMutNodeReplaceP;
}
#endif // TreeMutNodeReplace_h
