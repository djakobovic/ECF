#ifndef GEPCHROMOSOMEMUTOP_H
#define GEPCHROMOSOMEMUTOP_H
#include "../ECF_base.h"
#include "GEPChromosome.h"

namespace GEP{
	/**
	* \ingroup genotypes gep
	* \brief GEPChromosome genotype: node replacement mutation operator.
	* Tries to replace the selected primitive with a different one, conforming to the restrictions of the domain where it is located.
	*/
	class GEPChromosomeMutOp : public MutationOp
	{
	public:
		void registerParameters(StateP state);
		bool initialize(StateP state);
		bool mutate(GenotypeP gene);
	};
}
typedef std::shared_ptr<GEP::GEPChromosomeMutOp> GEPChromosomeMutOpP;
#endif // GEPChromosomeMutOp_h