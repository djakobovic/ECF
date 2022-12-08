#include "GepChromosomeCrsGene.h"

namespace GEP{
	void GEPChromosomeCrsGene::registerParameters(StateP state)
	{
		myGenotype_->registerParameter(state, "crx.gene", (voidP) new double(0), ECF::DOUBLE);
	}


	bool GEPChromosomeCrsGene::initialize(StateP state)
	{
		voidP sptr = myGenotype_->getParameterValue(state, "crx.gene");
		probability_ = *((double*)sptr.get());
		return true;
	}

	bool GEPChromosomeCrsGene::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child){
		// get the genotype from the parents and child
		GEPChromosome* p1 = (GEPChromosome*)(gen1.get());
		GEPChromosome* p2 = (GEPChromosome*)(gen2.get());
		GEPChromosome* ch = (GEPChromosome*)(child.get());
		// test whether gene crossover is viable (i.e., there is more than one gene)
		if (p1->genes < 2){
			ECF_LOG(state_, 5, "Gene crossover failed: chromosome consists of fewer than 2 genes");
			return true;
		}
		ch->clear();
		// select the gene number to be swapped
		uint geneCrs = state_->getRandomizer()->getRandomInteger(0, (int)p1->genes);
		uint geneLen = p1->geneLength;
		ECF_LOG(state_, 5, "Performing Gene crossover in gene ("+ uint2str(geneCrs) + ")...");
		// swap the selected gene between the parents
		uint jStop;
		switch (state_->getRandomizer()->getRandomInteger(0, 1)) {
		case 0: 
				for (uint i = 0; i <= ch->genes; i++) {
					jStop = (i == ch->genes) ? ch->linkHeadLength + ch->linkTailLength : geneLen;
					for (uint j = 0; j < jStop; j++){
						ch->push_back(static_cast<Tree::NodeP> (new Tree::Node(i == geneCrs ? p2->at(i*geneLen + j) : p1->at(i*geneLen + j))));
					}
				}
				break;
		case 1: 
			for (uint i = 0; i <= ch->genes; i++) {
				jStop = (i == ch->genes) ? ch->linkHeadLength + ch->linkTailLength : geneLen;
				for (uint j = 0; j < jStop; j++){
					ch->push_back(static_cast<Tree::NodeP> (new Tree::Node(i == geneCrs ? p1->at(i*geneLen + j) : p2->at(i*geneLen + j))));
				}
			}
		}

		return true;
	}
}