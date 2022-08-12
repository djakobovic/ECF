#ifndef BINARYMUTSIMPLE_H_
#define BINARYMUTSIMPLE_H_


namespace Binary
{

/**
 * \ingroup genotypes binary
 * \brief Binary genotype: simple (bit-flip) mutation operator
 */
class BinaryMutSimple: public MutationOp 
{
protected:
	double bitProb_;
	bool bUseBitProb_;

public:
	bool mutate(GenotypeP gene);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<BinaryMutSimple> BinaryMutSimpleP;
}

#endif /* BINARYMUTSIMPLE_H_ */

