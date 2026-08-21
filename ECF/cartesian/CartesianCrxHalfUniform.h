#ifndef CartesianCrsHalfUniform_h
#define CartesianCrsHalfUniform_h

#include "../ECF_base.h"


namespace Cartesian {
	/**
	 * \ingroup genotypes cgp
	 * \brief Cartesian genotype: half uniform crossover operator
	 * 
	 * Child is a copy of 1st parent; differing nodes from 2nd parent are taken with 50% probability
	 */
	class CartesianCrxHalfUniform: public CrossoverOp
	{
	public:
		bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
		bool initialize(StateP);
		void registerParameters(StateP);
	};
	typedef std::shared_ptr<CartesianCrxHalfUniform> CartesianCrxHalfUniformP;
}

#endif /* CartesianCrsHalfUniform_h */