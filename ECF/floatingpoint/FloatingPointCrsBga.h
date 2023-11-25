#ifndef FLOATINGPOINTCRSBGA_H_
#define FLOATINGPOINTCRSBGA_H_


namespace FloatingPoint
{

/**
 * \ingroup genotypes flpoint
 * \brief FloatingPoint genotype: BGA crossover (http://bib.irb.hr/datoteka/640222.CEC_2013.pdf,  http://sci2s.ugr.es/publications/ficheros/IJIS-2003-18-3-309-338.PDF)
 */
class FloatingPointCrsBga: public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);

};
typedef std::shared_ptr<FloatingPointCrsBga> FloatingPointCrsBgaP;
}
#endif /* FLOATINGPOINTCRSBGA_H_ */

