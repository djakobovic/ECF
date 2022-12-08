#include "GEPChromosomeCrsTwoPoint.h"

namespace GEP{
	void GEPChromosomeCrsTwoPoint::registerParameters(StateP state)
	{
		myGenotype_->registerParameter(state, "crx.twopoint", (voidP) new double(0), ECF::DOUBLE);
	}


	bool GEPChromosomeCrsTwoPoint::initialize(StateP state)
	{
		voidP sptr = myGenotype_->getParameterValue(state, "crx.twopoint");
		probability_ = *((double*)sptr.get());
		return true;
	}

	bool GEPChromosomeCrsTwoPoint::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child){
		// get the genotypes from the parents and child
		GEPChromosome* p1 = (GEPChromosome*)(gen1.get());
		GEPChromosome* p2 = (GEPChromosome*)(gen2.get());
		GEPChromosome* ch = (GEPChromosome*)(child.get());
		ch->clear();
		// select two points between which to perform the crossover
		uint bitCrs = state_->getRandomizer()->getRandomInteger(0, (int)p1->size() - 1);
		uint bitCrsSecond, tmp;
		do
		{
			bitCrsSecond = state_->getRandomizer()->getRandomInteger(p1->size());
		} while (bitCrs == bitCrsSecond);

		if (bitCrs>bitCrsSecond)
		{
			tmp = bitCrs;
			bitCrs = bitCrsSecond;
			bitCrsSecond = tmp;
		}
		ECF_LOG(state_, 5, "Performing Two-Pt crossover at nodes (" + uint2str(bitCrs) + ") and (" + uint2str(bitCrsSecond) + ")...");
		// swap the region delimited by the chosen points between the parents
		switch (state_->getRandomizer()->getRandomInteger(0, 1)) {
		case 0: 
				for (uint i = 0; i < bitCrs; i++) {
					ch->push_back(static_cast<Tree::NodeP> (new Tree::Node((p1->at(i)))));
				}
				for (uint i = bitCrs; i < bitCrsSecond; i++) {
					ch->push_back(static_cast<Tree::NodeP> (new Tree::Node((p2->at(i)))));
				}
				for (uint i = bitCrsSecond; i < p2->size(); i++) {
					ch->push_back(static_cast<Tree::NodeP> (new Tree::Node((p1->at(i)))));
				}
				break;
		case 1: 
				for (uint i = 0; i < bitCrs; i++) {
					ch->push_back(static_cast<Tree::NodeP> (new Tree::Node((p2->at(i)))));
				}
				for (uint i = bitCrs; i < bitCrsSecond; i++) {
					ch->push_back(static_cast<Tree::NodeP> (new Tree::Node((p1->at(i)))));
				}
				for (uint i = bitCrsSecond; i < p1->size(); i++) {
					ch->push_back(static_cast<Tree::NodeP> (new Tree::Node((p2->at(i)))));
				}
		}

		return true;
	}
}