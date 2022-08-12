#ifndef FLOATINGPOINTCRSDISCRETE_H_
#define FLOATINGPOINTCRSDISCRETE_H_


namespace FloatingPoint
{

/**
 * \ingroup genotypes flpoint
 * \brief FloatingPoint genotype: allele value for each gene is either from parent1 or from parent2 with equal probability
 */
class FloatingPointCrsDiscrete: public CrossoverOp 
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<FloatingPointCrsDiscrete> FloatingPointCrsDiscreteP;
}
#endif /* FLOATINGPOINTCRSDISCRETE_H_ */

