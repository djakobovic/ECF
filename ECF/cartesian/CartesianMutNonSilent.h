#ifndef CartesianMutNonSilent_h
#define CartesianMutNonSilent_h

#include "../ECF_base.h"

namespace Cartesian {

	/**
	  * \ingroup genotypes cgp
	  * Cartesian genotype: non silent mutation operator
	  * 
	  *	Non silent mutation transforms silent node (if there is one) into active by
	  * connecting silentNode to the one of the inputs of the active node 
	  * (column(activeNode) > column(silentNode)) or by connecting silentNode to the output.
	  */
	class CartesianMutNonSilent : public MutationOp
	{
	public:
		bool mutate(GenotypeP gene);
		bool initialize(StateP);
		void registerParameters(StateP);
	};
	typedef std::shared_ptr<CartesianMutNonSilent> CartesianMutNonSilentP;

}

#endif /* CartesianMutNonSilent_h */