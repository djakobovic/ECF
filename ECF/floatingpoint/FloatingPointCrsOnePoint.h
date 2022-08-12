#ifndef FLOATINGPOINTCRSONEPOINT_H_
#define FLOATINGPOINTCRSONEPOINT_H_


namespace FloatingPoint
{

/**
 * \ingroup genotypes flpoint
 * \brief FloatingPoint genotype: one point crossover operator with permissible split points only between dimensions
 */
class FloatingPointCrsOnePoint: public CrossoverOp 
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<FloatingPointCrsOnePoint> FloatingPointCrsOnePointP;
}
#endif /* FLOATINGPOINTCRSONEPOINT_H_ */

