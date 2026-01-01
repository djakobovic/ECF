#ifndef CartesianCrsHalfUniform_h
#define CartesianCrsHalfUniform_h

#include "ecf/ECF_base.h"


namespace Cartesian {
/**
 * \brief Cartesian genotype: half uniform crossover operator
 */
	class CartesianCrxHalfUniform: public CrossoverOp
	{
	public:
		/**
		Bits are randomly copied from both parents with equal probability
		*/
		bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
		bool initialize(StateP);
		void registerParameters(StateP);
	};
	typedef std::shared_ptr<CartesianCrxHalfUniform> CartesianCrxHalfUniformP;

}

#endif /* CartesianCrsHalfUniform_h */