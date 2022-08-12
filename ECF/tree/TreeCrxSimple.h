#ifndef TreeCrxSimple_h
#define TreeCrxSimple_h


namespace Tree
{

/**
 * \ingroup genotypes tree
 * \brief Tree genotype: simple tree crossover operator (with default 90% bias toward functional node)
 * Reference: http://dces.essex.ac.uk/staff/rpoli/gp-field-guide/24RecombinationandMutation.html#7_4
 */
class TreeCrxSimple : public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
protected:
	double funcChoiceProb_;
};
typedef boost::shared_ptr<TreeCrxSimple> TreeCrxSimpleP;
}
#endif // TreeCrxSimple_h
