#include <cmath>
#include <sstream>


namespace IntGenotype
{
	/**
	 * \ingroup intgenotype genotypes
	 * \brief IntGenotype genotype: single-point mutation operator
	 */
	class IntGenotypeMutOp : public MutationOp
	{
	public:
		bool mutate(GenotypeP gene);
	};
	typedef std::shared_ptr<IntGenotypeMutOp> IntGenotypeMutOpP;

}