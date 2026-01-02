#ifndef CartesianCrsUniform_h
#define CartesianCrsUniform_h

#include "../ECF_base.h"


namespace Cartesian {
/**
 * \brief Cartesian genotype: uniform crossover operator
 */
	class CartesianCrxUniform: public CrossoverOp
	{
	public:
		/**
		Genes are randomly copied from both parents with equal probability
		*/
		bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
		bool initialize(StateP);
		void registerParameters(StateP);
	};
	typedef std::shared_ptr<CartesianCrxUniform> CartesianCrxUniformP;

}

#endif /* CartesianCrsUniform_h */
