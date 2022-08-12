#ifndef TreeCrxSimple_h
#define TreeCrxSimple_h

/**
 * \brief Tree genotype: simple tree crossover operator.
 * Reference: http://dces.essex.ac.uk/staff/rpoli/gp-field-guide/24RecombinationandMutation.html#7_4
 */
class TreeCrxSimple : public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<TreeCrxSimple> TreeCrxSimpleP;

#endif // TreeCrxSimple_h


