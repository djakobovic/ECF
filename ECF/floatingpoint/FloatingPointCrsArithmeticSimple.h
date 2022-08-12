#ifndef FLOATINGPOINTCRSARITHMETICSIMPLE_H_
#define FLOATINGPOINTCRSARITHMETICSIMPLE_H_


namespace FloatingPoint
{

/**
 * \ingroup genotypes flpoint
 * \brief FloatingPoint genotype: take recombination point k. Child 1 is parent1 until k, rest is arithmetic average of parents
 */
class FloatingPointCrsArithmeticSimple: public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<FloatingPointCrsArithmeticSimple> FloatingPointCrsArithmeticSimpleP;
}
#endif /* FLOATINGPOINTCRSARITHMETICSIMPLE_H_ */

