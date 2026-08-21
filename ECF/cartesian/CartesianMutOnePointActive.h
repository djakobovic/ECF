#ifndef CartesianMutOnePointActive_h
#define CartesianMutOnePointActive_h

#include "../ECF_base.h"

namespace Cartesian {

	/**
	  * \ingroup genotypes cgp
	  * Cartesian genotype: one point active mutation
	  *
	  *	Mutation point is randomly chosen among active genes in genotype.
	  * Chosen mutation point might be function or output node. 
	  */
	class CartesianMutOnePointActive : public MutationOp
	{
	public:
		bool mutate(GenotypeP gene);
		bool initialize(StateP);
		void registerParameters(StateP);
	};
	typedef std::shared_ptr<CartesianMutOnePointActive> CartesianMutOnePointActiveP;

}

#endif /* CartesianMutOnePointActive */
