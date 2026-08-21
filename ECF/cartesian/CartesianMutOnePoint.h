#ifndef CartesianMutOnePoint_h
#define CartesianMutOnePoint_h

#include "../ECF_base.h"


namespace Cartesian {

/**
  * \ingroup genotypes cgp
  * Cartesian genotype: one point mutation operator
  * Mutation point is randomly chosen among all genes in genotype (active or not). 
  * Chosen mutation point might be function or output node. 
  */
class CartesianMutOnePoint: public MutationOp 
{
public:
	bool mutate(GenotypeP gene);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef std::shared_ptr<CartesianMutOnePoint> CartesianMutOnePointP;

}

#endif /* CartesianMutOnePoint_h */