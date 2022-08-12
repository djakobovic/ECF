#ifndef BitStringUniformCrs_h
#define BitStringUniformCrs_h


namespace BitString
{

/**
 * \ingroup bitstring genotypes
 * \brief BitString genotype uniform crossover operator
 */
class BitStringCrsUniform : public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);	
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<BitStringCrsUniform> BitStrCrsUniformP;
}
#endif // BitStringUniformCrs_h

