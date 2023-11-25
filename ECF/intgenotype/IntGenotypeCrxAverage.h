#include <cmath>
#include <sstream>

namespace IntGenotype
{
	/**
	 * \ingroup intgenotype genotypes
	 * \brief IntGenotype genotype: average crossover operator
	 */
	class IntGenotypeCrxAverage : public CrossoverOp
	{
	public:
		bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
		
	};
	typedef std::shared_ptr<IntGenotypeCrxAverage> IntGenotypeCrxAverageP;

}