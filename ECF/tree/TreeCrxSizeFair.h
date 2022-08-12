#ifndef TreeCrxSizeFair_h
#define TreeCrxSizeFair_h


namespace Tree
{

/**
 * \ingroup genotypes tree
 * \brief Tree genotype: size fair crx operator.
 * Reference: http://dces.essex.ac.uk/staff/rpoli/gp-field-guide/53GPCrossover.html#11_3
 */
class TreeCrxSizeFair : public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	int calculateSize(int avg);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<TreeCrxSizeFair> TreeCrxSizeFairP;
}
#endif // TreeCrxSizeFair_h
