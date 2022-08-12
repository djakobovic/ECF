#ifndef TreeCrxContextPreserved_h
#define TreeCrxContextPreserved_h


namespace Tree
{

/**
 * \ingroup genotypes tree
 * \brief Tree genotype: context presevation crx operator.
 * Tries to make crossover at the 'same' point in both trees (with the same path from tree root node).
 * Reference: http://dces.essex.ac.uk/staff/rpoli/gp-field-guide/53GPCrossover.html#11_3
 */
class TreeCrxContextPreserved : public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<TreeCrxContextPreserved> TreeCrxContextPreservedP;
}
#endif // TreeCrxContextPreserved_h
