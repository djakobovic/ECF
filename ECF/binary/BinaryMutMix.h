#ifndef BINARYMUT_H_
#define BINARYMUT_H_


namespace Binary
{

/**
 * \ingroup genotypes binary
 * \brief Binary genotype: mixing mutation operator
 */
class BinaryMutMix: public MutationOp 
{
public:
	bool mutate(GenotypeP gene);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<BinaryMutMix> BinaryMutMixP;
}

#endif /* BINARYMUT_H_ */

