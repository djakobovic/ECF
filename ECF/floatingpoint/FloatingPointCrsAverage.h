#ifndef FLOATINGPOINTCRSAVERAGE_H_
#define FLOATINGPOINTCRSAVERAGE_H_


namespace FloatingPoint
{

/**
 * \ingroup genotypes flpoint
 * \brief FloatingPoint genotype: child is average value of parent genes
 */
class FloatingPointCrsAverage: public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef std::shared_ptr<FloatingPointCrsAverage> FloatingPointCrsAverageP;
}
#endif /* FLOATINGPOINTCRSAVERAGE_H_ */

