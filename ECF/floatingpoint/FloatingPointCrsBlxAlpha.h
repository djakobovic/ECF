#ifndef FLOATINGPOINTCRSBLXALPHA_H_
#define FLOATINGPOINTCRSBLXALPHA_H_


namespace FloatingPoint
{

/**
 * \ingroup genotypes flpoint
 * \brief FloatingPoint genotype: BLX alpha crossover (http://bib.irb.hr/datoteka/640222.CEC_2013.pdf)
 */
class FloatingPointCrsBlxAlpha: public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);

	double alpha;
};
typedef std::shared_ptr<FloatingPointCrsBlxAlpha> FloatingPointCrsBlxAlphaP;
}
#endif /* FLOATINGPOINTCRSBLXALPHA_H_ */

