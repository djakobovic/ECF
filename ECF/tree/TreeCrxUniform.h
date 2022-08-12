#ifndef TreeCrxUniform_h
#define TreeCrxUniform_h


namespace Tree
{

/**
 * \ingroup genotypes tree
 * \brief Tree genotype: uniform crx operator.
 * Reference: http://dces.essex.ac.uk/staff/rpoli/gp-field-guide/53GPCrossover.html#11_3
 */
class TreeCrxUniform : public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<TreeCrxUniform> TreeCrxUniformP;
}
#endif // TreeCrxUniform_h
