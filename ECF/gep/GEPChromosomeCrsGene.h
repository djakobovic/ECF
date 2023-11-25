#ifndef GEPCHROMOSOMECRSGENE_H
#define GEPCHROMOSOMECRSGENE_H
#include "../ECF_base.h"
#include "GEPChromosome.h"

namespace GEP
{
	/**
	* \ingroup genotypes gep
	* \brief GEPChromosome genotype: gene crx operator.
	* Selects a gene number and swaps it between both parents
	*/

	class GEPChromosomeCrsGene : public CrossoverOp
	{
	public:
		bool initialize(StateP);
		void registerParameters(StateP);
		bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	};
	typedef std::shared_ptr<GEPChromosomeCrsGene> GEPChromosomeCrsGeneP;
}
#endif // GEPChromosomeCrsGene_h