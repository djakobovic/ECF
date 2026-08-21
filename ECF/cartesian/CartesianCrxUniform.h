#ifndef CartesianCrsUniform_h
#define CartesianCrsUniform_h

#include "../ECF_base.h"


namespace Cartesian {
/**
 * \ingroup genotypes cgp
 * \brief Cartesian genotype: uniform crossover operator
 * 
 * Genes are randomly copied from both parents with equal probability
 */
	class CartesianCrxUniform: public CrossoverOp
	{
	public:
		bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
		bool initialize(StateP);
		void registerParameters(StateP);
	};
	typedef std::shared_ptr<CartesianCrxUniform> CartesianCrxUniformP;
}

#endif /* CartesianCrsUniform_h */
