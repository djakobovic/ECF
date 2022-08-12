#ifndef BINARYCRSONEPOINT_H_
#define BINARYCRSONEPOINT_H_


namespace Binary
{


/**
 * \brief Binary genotype: one point crossover operator
 * \ingroup binary genotypes
 */
class BinaryCrsOnePoint: public CrossoverOp 
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<BinaryCrsOnePoint> BinaryCrsOnePointP;
}

#endif /* BINARYCRSONEPOINT_H_ */

