#include "GEPChromosomeMutOp.h"

namespace GEP{
	void GEPChromosomeMutOp::registerParameters(StateP state)
	{
		myGenotype_->registerParameter(state, "mut.simple", (voidP) new double(0), ECF::DOUBLE);
	}


	bool GEPChromosomeMutOp::initialize(StateP state)
	{
		voidP sptr = myGenotype_->getParameterValue(state, "mut.simple");
		probability_ = *((double*)sptr.get());

		return true;
	}
	bool GEPChromosomeMutOp::mutate(GenotypeP gene){
		GEPChromosome* chr = (GEPChromosome*)(gene.get());
		// mutate a single random point in the genotype by selecting a random primitive
		// Select a random gene
		uint iGene = state_->getRandomizer()->getRandomInteger(0, chr->genes - 1);
		uint geneOffset = iGene * chr->geneLength;
		// Select a random point in the head+tail
		uint iPoint = state_->getRandomizer()->getRandomInteger(0, (uint)(chr->headLength+chr->tailLength) - 1);
		std::stringstream logstr;
		logstr << "Mutating node (" << iPoint << ") in gene [" << iGene << "]...";
		ECF_LOG(state_, 5, logstr.str());
		// get primitive depending on where the selected point is
		// head points can change into anything; tail points must be terminals
		chr->at(geneOffset + iPoint)->setPrimitive(iPoint < chr->headLength ? chr->primitiveSet_->getRandomPrimitive() : chr->primitiveSet_->getRandomTerminal());
		return true;
	}
}