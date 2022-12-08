#include "GEPChromosomeMutGauss.h"

namespace GEP{
	void GEPChromosomeMutGauss::registerParameters(StateP state)
	{
		myGenotype_->registerParameter(state, "mut.gauss", (voidP) new double(0), ECF::DOUBLE);
	}

	bool GEPChromosomeMutGauss::initialize(StateP state)
	{
		voidP sptr = myGenotype_->getParameterValue(state, "mut.gauss");
		probability_ = *((double*)sptr.get());
		engine_.seed((uint)time(NULL));
		return true;
	}

	bool GEPChromosomeMutGauss::mutate(GenotypeP gene){
		GEPChromosome* chr = (GEPChromosome*)(gene.get());
		if (chr->dcLength > 0){
			// mutate a single random point in the Dc domain by adding Gaussian noise to it
			// Select a random gene
			uint iGene = state_->getRandomizer()->getRandomInteger(0, chr->genes - 1);
			uint geneOffset = iGene * chr->geneLength;
			uint constOffset = geneOffset + chr->headLength + chr->tailLength; // this is where the Dc of the selected gene begins
			// Select a random point in the Dc tail
			uint iPoint = constOffset + state_->getRandomizer()->getRandomInteger(0, (uint)(chr->dcLength) - 1);

			double oldValue;
			Tree::PrimitiveP oldPrim = chr->at(iPoint)->primitive_;
			chr->at(iPoint)->primitive_->getValue(&oldValue);
			std::string oldName = chr->at(iPoint)->primitive_->getName();

			// generate Gauss noise offset and add it
			boost::normal_distribution<double> N(0, 1);
			double offset = N.operator () < boost::lagged_fibonacci607 > (engine_);
			double newValue = oldValue + offset;

			// change double ERC value and name
			std::stringstream ss;
			ss << newValue;
			std::string newName;
			ss >> newName;
			newName = DBL_PREFIX + newName;

			oldPrim->setName(newName);
			oldPrim->setValue(&newValue);
			// print result
			std::stringstream log;
			log << "GEPMutGauss successful (oldNode = " << oldName << ", newNode = " << newName << ")";
			ECF_LOG(state_, 5, log.str());
		}
		return true;
	}
}