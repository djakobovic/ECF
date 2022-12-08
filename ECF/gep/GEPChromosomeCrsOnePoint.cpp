#include "GEPChromosomeCrsOnePoint.h"

namespace GEP{
	void GEPChromosomeCrsOnePoint::registerParameters(StateP state)
	{
		myGenotype_->registerParameter(state, "crx.onepoint", (voidP) new double(0), ECF::DOUBLE);
	}


	bool GEPChromosomeCrsOnePoint::initialize(StateP state)
	{
		voidP sptr = myGenotype_->getParameterValue(state, "crx.onepoint");
		probability_ = *((double*)sptr.get());
		return true;
	}

	bool GEPChromosomeCrsOnePoint::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child){
		// get the genotype from the parents and the child
		GEPChromosome* p1 = (GEPChromosome*)(gen1.get());
		GEPChromosome* p2 = (GEPChromosome*)(gen2.get());
		GEPChromosome* ch = (GEPChromosome*)(child.get());
		ch->clear();
		// select a point on which the crossover operator will be applied
		uint bitCrs = state_->getRandomizer()->getRandomInteger(0, (int)p1->size() - 1);
		ECF_LOG(state_, 5, "Performing One-Pt crossover at node (" + uint2str(bitCrs) + ")...");
		// Copy the nodes from one parent until the crossover point, then the nodes from the other parent until the end
		switch (state_->getRandomizer()->getRandomInteger(0, 1)) {
		case 0: for (uint i = 0; i < bitCrs; i++) {
			ch->push_back(static_cast<Tree::NodeP> (new Tree::Node((p1->at(i)))));
		}
				for (uint i = bitCrs; i < p2->size(); i++) {
					ch->push_back(static_cast<Tree::NodeP> (new Tree::Node((p2->at(i)))));
				}
				break;
		case 1: for (uint i = 0; i < bitCrs; i++) {
			ch->push_back(static_cast<Tree::NodeP> (new Tree::Node((p2->at(i)))));
		}
				for (uint i = bitCrs; i < p1->size(); i++) {
					ch->push_back(static_cast<Tree::NodeP> (new Tree::Node((p1->at(i)))));
				}
		}

		return true;
	}
}