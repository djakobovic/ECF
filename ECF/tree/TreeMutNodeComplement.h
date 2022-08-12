#ifndef TreeMutNodeComplement_h
#define TreeMutNodeComplement_h


namespace Tree
{

/**
 * \ingroup genotypes tree
 * \brief Tree genotype: complement node mutation operator.
 * For the operator to succeed, the chosen primitive must have a defined complement.
 */
class TreeMutNodeComplement : public MutationOp
{
public:
	bool mutate(GenotypeP gene);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<TreeMutNodeComplement> TreeMutNodeComplementP;
}
#endif // TreeMutNodeComplement_h
