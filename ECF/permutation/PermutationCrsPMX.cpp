#include "../ECF_base.h"
#include "Permutation.h"
#include <map>


namespace Permutation
{

void PermutationCrsPMX::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.PMX", (voidP) new double(0), ECF::DOUBLE);
}


bool PermutationCrsPMX::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.PMX");
	probability_ = *((double*)sptr.get());
	return true;
}

	
bool PermutationCrsPMX::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	Permutation* p1 = (Permutation*) (gen1.get());
	Permutation* p2 = (Permutation*) (gen2.get());
	Permutation* ch = (Permutation*) (child.get());

	uint start = state_->getRandomizer()->getRandomInteger(p1->getSize());
	uint end = state_->getRandomizer()->getRandomInteger((int) start, (int) p1->getSize() - 1);

	std::map<int, int> mappings;

	for(uint i = start; i < end; i++) {
		ch->variables[i] = p1->variables[i];
		mappings[p1->variables[i]] = p2->variables[i];
	}
	
	std::map<int, int>::iterator iter = mappings.begin();

	for(uint i = 0; i < p1->getSize(); i++) {
		//ako smo unutar podniza nemamo sto raditi sa tim elementima
		if(i >= start && i < end)
			continue;
		//za pocetak probajmo s elementom iz drugog roditelja
		int map = p2->variables[i];
		//dok god se trenutni element koji želimo staviti na i-to mjesto 
		//djeteta vec u njemu nalazi trazimo dalje
		do {
			iter = mappings.find(map);
			//ako se nalazi vec u podnizu probajmo s elementom koji je prije stajao na tom mjestu
			if(iter != mappings.end()) {
				map = iter->second;
			}
			else 
				break;
		} while(true);
		ch->variables[i] = map;
	}

	return true;
}

}