#ifndef BitStringCrsOnePoint_h
#define BitStringCrsOnePoint_h


namespace BitString
{

/**
 * \ingroup bitstring genotypes
 * \brief BitString genotype: one point crossover operator
 */
class BitStringCrsOnePoint : public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<BitStringCrsOnePoint> BitStrCrsOnePointP;
}
#endif // BitStringCrsOnePoint_h

