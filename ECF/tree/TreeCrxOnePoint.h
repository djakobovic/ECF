#ifndef TreeCrxOnePoint_h
#define TreeCrxOnePoint_h


namespace Tree
{

/**
 * \ingroup genotypes tree
 * \brief Tree genotype: one point crx operator.
 * Tries to select a crossing point in parent tree's common region.
 * Reference: http://dces.essex.ac.uk/staff/rpoli/gp-field-guide/53GPCrossover.html#11_3
 */
class TreeCrxOnePoint : public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<TreeCrxOnePoint> TreeCrxOnePointP;
}
#endif // TreeCrxOnePoint_h
