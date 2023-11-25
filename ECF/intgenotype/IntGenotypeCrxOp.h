#include <cmath>

namespace IntGenotype
{
	/**
	 * \ingroup intgenotype genotypes
	 * \brief IntGenotype genotype: uniform crossover operator
	 */
	class IntGenotypeCrxOp : public CrossoverOp
	{
	public:
		bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
		
	};
	typedef std::shared_ptr<IntGenotypeCrxOp> IntGenotypeCrxOpP;

}