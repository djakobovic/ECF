#include "CartesianMutOnePoint.h"
#include "Cartesian_c.h"
#include "FunctionSet.h"
#include <cmath>
#include <algorithm>

namespace Cartesian
{

	void CartesianMutOnePoint::registerParameters(StateP state)
	{
		myGenotype_->registerParameter(state, "mut.onepoint", (voidP) new double(0), ECF::DOUBLE);
	}


	bool CartesianMutOnePoint::initialize(StateP state)
	{
		voidP sptr = myGenotype_->getParameterValue(state, "mut.onepoint");
		probability_ = *((double*)sptr.get());
		return true;
	}

	/*
	 * Reimplementation of mutation at one point.
	 * One point mutation needs to distinct between function gene or connection gene.
	 * If a gene is a function gene, new function gene must be a function which takes
	 * same number of arguments.
	 * If a gene is a connection gene mutation must be such that connection is valid.(backwards looking)
	 * */
	bool CartesianMutOnePoint::mutate(GenotypeP gene)
	{
		Cartesian* cart = (Cartesian*)(gene.get());
		RandomizerP randP = cart->state_->getRandomizer();
		std::vector<uint> functionGenesIndecies;
		FunctionSet *functionSet = (FunctionSet*)cart->functionSet.get();

		for(uint i = 0; i < cart->size() - cart->nOutputs; i++) {
			functionGenesIndecies.push_back(i);
			i+=functionSet->vFunctions[cart->operator[](i)]->getNumberOfArguments();
		}


		const uint randomGeneIndex = randP->getRandomInteger(0, cart->size() - cart->nOutputs); // Gene that will be mutated
		bool match = false;
		for(uint i = 0; i < functionGenesIndecies.size(); i++) {
			if(functionGenesIndecies[i] == randomGeneIndex) {
				match = true;
				break;
			}
		}
		//Changing function gene.
		if(match) {
			int findex = randP->getRandomInteger(0, functionSet->vFunctions.size()-1);
			while(functionSet->vFunctions[findex]->getNumberOfArguments() != functionSet->vFunctions[cart->operator[](randomGeneIndex)]->getNumberOfArguments()) {
				findex = randP->getRandomInteger(0, functionSet->vFunctions.size()-1);
			}
			cart->at(randomGeneIndex) = findex;
		}
		else {
			int findex = 0;
			for(uint i = 0; i < functionGenesIndecies.size(); i++) {
				if(functionGenesIndecies[i] < randomGeneIndex) {
					findex = i;
				}
				else {
					break;
				}
			}
			//findex now has a index location of a operator. This is needed for levels back and calculations of rewiring connection gene
			int rowindex = 0;
			int columnsum = cart->nCols - 1;
			while(findex > columnsum) {
				rowindex++;
				columnsum += (cart->nCols -1);
			}
			cart->at(randomGeneIndex) = cart->randomConnectionGenerator(rowindex);
		}

		return true;
	}

}



