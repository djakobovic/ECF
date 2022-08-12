#ifndef BINARYSEGMENTEDCRS_H_
#define BINARYSEGMENTEDCRS_H_


namespace Binary
{

/**
 * \ingroup genotypes binary
 * \brief Binary genotype: segmented crossover operator
 */
class BinaryCrsSegmented: public CrossoverOp 
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<BinaryCrsSegmented> BinaryCrsSegmentedP;
}

#endif /* BINARYSEGMENTEDCRS_H_ */

