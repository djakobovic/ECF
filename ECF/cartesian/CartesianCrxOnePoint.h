#ifndef CartesianCrxOnePoint_h
#define CartesianCrxOnePoint_h

#include "../ECF_base.h"


namespace Cartesian {
/**
 * \ingroup genotypes cgp
 * \brief Cartesian genotype: one point crossover operator
 * 
 * 	Crossing point is determined randomly between all function nodes in genotype.
 */
class CartesianCrxOnePoint: public CrossoverOp 
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef std::shared_ptr<CartesianCrxOnePoint> CartesianCrxOnePointP;

}

#endif /* CartesianCrxOnePoint_h */
