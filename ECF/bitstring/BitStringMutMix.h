#ifndef BitStringMut_h
#define BitStringMut_h


namespace BitString
{

/**
 * \ingroup bitstring genotypes
 * \brief BitString genotype mixing mutation operator
 */
class BitStringMutMix : public MutationOp
{
public:
	bool mutate(GenotypeP gene);	
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<BitStringMutMix> BitStringMutMixP;
}
#endif // BitStringMut_h

