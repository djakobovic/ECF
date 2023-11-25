#include <cmath>
#include <sstream>

namespace IntGenotype
{
	/**
	 * \ingroup intgenotype genotypes
	 * \brief IntGenotype genotype: two-point crossover operator
	 */
	class IntGenotypeCrxTwoPoint : public CrossoverOp
	{
	public:
		bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
		
	};
	typedef std::shared_ptr<IntGenotypeCrxTwoPoint> IntGenotypeCrxTwoPointP;

}