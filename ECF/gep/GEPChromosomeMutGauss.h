#ifndef GEPCHROMOSOMEMUTGAUSS_H
#define GEPCHROMOSOMEMUTGAUSS_H
#include "../ECF_base.h"
#include <time.h>
#include "GEPChromosome.h"
//#include "boost/random/lagged_fibonacci.hpp"
#include <random>

namespace GEP{
	/**
	* \ingroup genotypes gep
	* \brief GEPChromosome genotype: standard normal distribution noise mutation operator.
	* Applicable only on ephemereal random constants (ERC) of type 'double'.
	*/
	class GEPChromosomeMutGauss : public MutationOp
	{
	public:
		void registerParameters(StateP state);
		bool initialize(StateP state);
		bool mutate(GenotypeP gene);
	protected:
		//boost::lagged_fibonacci607 engine_;
		std::mt19937 engine_;
	};
}
typedef std::shared_ptr<GEP::GEPChromosomeMutGauss> GEPChromosomeMutGaussP;
#endif // GEPChromosomeMutGauss_h