#ifndef BINARYCRSTWOPOINT_H_
#define BINARYCRSTWOPOINT_H_


namespace Binary
{


/**
 * \ingroup genotypes binary
 * \brief Binary genotype: two point crossover operator
 */
class BinaryCrsTwoPoint: public CrossoverOp 
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<BinaryCrsTwoPoint> BinaryCrsTwoPointP;
}

#endif /* BINARYCRSTWOPOINT_H_ */

