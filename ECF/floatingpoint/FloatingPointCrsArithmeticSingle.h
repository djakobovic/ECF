#ifndef FLOATINGPOINTCRSARITHMETICSINGLE_H_
#define FLOATINGPOINTCRSARITHMETICSINGLE_H_


namespace FloatingPoint
{

/**
 * \ingroup genotypes flpoint
 * \brief FloatingPoint genotype: take random allele k. That point is arithmetic average of parents, other points are from parents
 */
class FloatingPointCrsArithmeticSingle: public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef std::shared_ptr<FloatingPointCrsArithmeticSingle> FloatingPointCrsArithmeticSingleP;
}
#endif /* FLOATINGPOINTCRSARITHMETICSINGLE_H_ */

