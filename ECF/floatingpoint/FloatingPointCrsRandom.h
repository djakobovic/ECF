#ifndef FLOATINGPOINTCRSRANDOM_H_
#define FLOATINGPOINTCRSRANDOM_H_


namespace FloatingPoint
{

/**
 * \ingroup genotypes flpoint
 * \brief FloatingPoint genotype: random crossover, for testing purposes
 */
class FloatingPointCrsRandom: public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef std::shared_ptr<FloatingPointCrsRandom> FloatingPointCrsRandomP;
}
#endif /* FLOATINGPOINTCRSRANDOM_H_ */

