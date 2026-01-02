#ifndef CartesianCrsOnePoint_h
#define CartesianCrsOnePoint_h

#include "../ECF_base.h"


namespace Cartesian {
/**
 * \brief Cartesian genotype: one point crossover operator
 */
class CartesianCrxOnePoint: public CrossoverOp 
{
public:
	/**
	Crossing point is determined randomly between all integer values in genotype.
	Left side of one parent from beginning of genotype until crossing point and right side of second parent 
	from crossing point until the end of genotype become child genotype.
	Order of sides taken in child genotype is randomly chosen.
	*/
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef std::shared_ptr<CartesianCrxOnePoint> CartesianCrsOnePointP;

}

#endif /* CartesianCrsOnePoint_h */
