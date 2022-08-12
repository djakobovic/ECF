#ifndef BitStringMutSimple_h
#define BitStringMutSimple_h


namespace BitString
{

/**
 * \ingroup bitstring genotypes
 * \brief BitString genotype simple (one bit) mutation operator
 */
class BitStringMutSimple : public MutationOp
{
protected:
	double bitProb_;     ///< probability of single bit mutation
	bool bUseBitProb_;   ///< are we using bit mutation probabiltiy (instead of individual probability)

public:
	bool mutate(GenotypeP gene);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<BitStringMutSimple> BitStringMutSimpleP;
}
#endif // BitStringMutSimple_h

