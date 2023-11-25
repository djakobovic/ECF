#ifndef FLOATINGPOINTCRSBLXALPHABETA_H_
#define FLOATINGPOINTCRSBLXALPHABETA_H_


namespace FloatingPoint
{

/**
 * \ingroup genotypes flpoint
 * \brief FloatingPoint genotype: BLX alpha-beta crossover (http://bib.irb.hr/datoteka/640222.CEC_2013.pdf)
 */
class FloatingPointCrsBlxAlphaBeta: public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);

	double alpha;
	double beta;
};
typedef std::shared_ptr<FloatingPointCrsBlxAlphaBeta> FloatingPointCrsBlxAlphaBetaP;
}
#endif /* FLOATINGPOINTCRSBLXALPHABETA_H_ */

