#ifndef FLOATINGPOINTCRSBLX_H_
#define FLOATINGPOINTCRSBLX_H_


namespace FloatingPoint
{

/**
 * \brief FloatingPoint genotype: value on allele i is random value taken from min-max interval from parents plus/minus difference times rand 
 */
class FloatingPointCrsBlx: public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
	double alpha;
	bool useAlpha;
};
typedef boost::shared_ptr<FloatingPointCrsBlx> FloatingPointCrsBlxP;
}
#endif /* FLOATINGPOINTCRSBLX_H_ */

