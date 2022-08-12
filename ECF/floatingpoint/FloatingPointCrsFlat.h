#ifndef FLOATINGPOINTCRSFLAT_H_
#define FLOATINGPOINTCRSFLAT_H_


namespace FloatingPoint
{

/**
 * \ingroup genotypes flpoint
 * \brief FloatingPoint genotype: value on allele i is random value taken from min-max interval from parents
 */
class FloatingPointCrsFlat: public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<FloatingPointCrsFlat> FloatingPointCrsFlatP;
}
#endif /* FLOATINGPOINTCRSFLAT_H_ */

