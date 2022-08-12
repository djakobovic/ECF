#ifndef FLOATINGPOINTCRSLOCAL_H_
#define FLOATINGPOINTCRSLOCAL_H_


namespace FloatingPoint
{

/**
 * \ingroup genotypes flpoint
 * \brief FloatingPoint genotype: local crossover (http://bib.irb.hr/datoteka/640222.CEC_2013.pdf)
 */
class FloatingPointCrsLocal: public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<FloatingPointCrsLocal> FloatingPointCrsLocalP;
}
#endif /* FLOATINGPOINTCRSLOCAL_H_ */

