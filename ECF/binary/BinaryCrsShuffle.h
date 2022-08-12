#ifndef BINARYCRSSHUFFLE_H_
#define BINARYCRSSHUFFLE_H_


namespace Binary
{


/**
 * \ingroup genotypes binary
 * \brief Binary genotype: shuffle crossover operator
 */
class BinaryCrsShuffle: public CrossoverOp 
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<BinaryCrsShuffle> BinaryCrsShuffleP;
}

#endif /* BINARYCRSSHUFFLE_H_ */

