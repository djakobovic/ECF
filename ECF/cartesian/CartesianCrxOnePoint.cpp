#include "CartesianCrxOnePoint.h"
#include "Cartesian_c.h"
#include "FunctionSet.h"

namespace Cartesian {

	void CartesianCrxOnePoint::registerParameters(StateP state)
	{
		myGenotype_->registerParameter(state, "crx.onepoint", (voidP) new double(0), ECF::DOUBLE);

	}


	bool CartesianCrxOnePoint::initialize(StateP state)
	{
		voidP sptr = myGenotype_->getParameterValue(state, "crx.onepoint");
		probability_ = *((double*)sptr.get());
		return true;
	}


	/* Remodeled crossover so that child genome can not be invalid.
		   This remodeled crossover assures that when a point is chosen, first gene is copied so that
		   it captures entire block (block here means an operator and correct number of operands).
	*/
	bool CartesianCrxOnePoint::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
	{
		Cartesian* p1 = (Cartesian*) (gen1.get());
		Cartesian* p2 = (Cartesian*) (gen2.get());
		Cartesian* ch = (Cartesian*) (child.get());
		ch->clear();
		int outputs1 = p1->nOutputs;
		int outputs2 = p2->nOutputs;
		std::vector<int> blocks1;
		std::vector<int> blocks2;
		std::vector<FunctionP> v1 = ((FunctionSet*)(p1->functionSet.get()))->vFunctions;
		std::vector<FunctionP> v2 = ((FunctionSet*)(p2->functionSet.get()))->vFunctions;
		//Indexing safe cut points
		for(uint i = 0; i < p1->size() - outputs1; i++) {
			blocks1.push_back(i);
			//i += v1[p1->operator[](i)]->getNumberOfArguments();
			// workaround dok se genotip ne izradi kao vektor gena:
			i += p1->maxArity;
		}
		for(uint i = 0; i < p2->size() - outputs2; i++) {
			blocks2.push_back(i);
			//i += v2[p2->operator[](i)]->getNumberOfArguments();
			// workaround dok se genotip ne izradi kao vektor gena:
			i += p2->maxArity;
		}
		int cutoff = blocks1[state_->getRandomizer()->getRandomInteger(1, blocks1.size() - 1)];
		int secondGeneCutoff = 0;
		int counter1 = 0;
		int counter2 = 0;
		//adjust cutoff to cut entire block

		for(uint i = 0; i < blocks1.size(); i++) {
			if(blocks1[i] >= cutoff) {
				cutoff = blocks1[i];
				counter1++;
				break;
			}
		}
		for(uint i = 0; i < blocks2.size(); i++) {
			if(blocks2[i] >= cutoff) {
				secondGeneCutoff = blocks2[i];
				counter2++;
				break;
			}
		}
		for(int i = 0; i < cutoff; i++) {
			ch->push_back(p1->at(i));
		}
		for(uint i = secondGeneCutoff; i < p2->size() - outputs2; i++) {
			ch->push_back(p2->at(i));
		}
		for(uint i = p2->size() - outputs2; i < p2->size(); i++) {
			ch->push_back(p2->at(i) + (counter1-counter2));
		}
		return true;
	}

}
