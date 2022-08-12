#include "../ECF_base.h"
#include "Binary.h"
#include <algorithm>

namespace Binary
{

void BinaryCrsShuffle::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.shuffle", (voidP) new double(0), ECF::DOUBLE);
}


bool BinaryCrsShuffle::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.shuffle");
	probability_ = *((double*)sptr.get());
	return true;
}
	
bool BinaryCrsShuffle::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	Binary* p1 = (Binary*) (gen1.get());
	Binary* p2 = (Binary*) (gen2.get());
	Binary* ch = (Binary*) (child.get());

	Binary* p1copy = (Binary*) (gen1->copy());
	Binary* p2copy = (Binary*) (gen2->copy());
	Binary* chcopy = (Binary*) (child->copy());

	//uint rand1, rand2;
	//bool temp;
	typedef std::vector<uint> v_uint;

	//v_uint pFrom;
	//v_uint pTo;

	v_uint mapiranje;

	for (uint i = 0; i < p1->getNumBits(); i++) { //this is initial arrangement
		mapiranje.push_back(i);
	}

	std::random_shuffle(mapiranje.begin(), mapiranje.end()); //jel moze ovo

	uint bitCrs = state_->getRandomizer()->getRandomInteger(p1->getNumBits());


	//sada citam redom sto pise u vectoru mapa i u kopije roditelja spremam na novu poziciju vrijednosti iz originalnih roditelja

	for (uint dimension = 0; dimension < p1->variables.size(); dimension++) {
	//shuffle parents
		for (uint i = 0; i < p1->getNumBits(); i++) {
			
			p1copy->variables[dimension][i] = p1->variables[dimension][mapiranje[i]];
			p2copy->variables[dimension][i] = p2->variables[dimension][mapiranje[i]];
		}

		//shuffle done, now just one-point crossover
		switch (state_->getRandomizer()->getRandomInteger(0, 1)) {
			case 0: for (uint i = 0; i < bitCrs; i++) {
						chcopy->variables[dimension][i] = p1copy->variables[dimension][i];
					}
					for (uint i = bitCrs; i < p2->getNumBits(); i++) {
						chcopy->variables[dimension][i] = p2copy->variables[dimension][i];
					}
					break;
			case 1: for (uint i = 0; i < bitCrs; i++) {
						chcopy->variables[dimension][i] = p2copy->variables[dimension][i];
					}
					for (uint i = bitCrs; i < p1->getNumBits(); i++) {
						chcopy->variables[dimension][i] = p1copy->variables[dimension][i];
					}
		}
		//unshuffle child
		for (uint i = 0; i < p1->getNumBits(); i++) {
			ch->variables[dimension][mapiranje[i]] = chcopy->variables[dimension][i];
		}
	}

	// update real domain representation
	ch->update();

	delete p1copy;
	delete p2copy;
	delete chcopy;

	return true;
}

}