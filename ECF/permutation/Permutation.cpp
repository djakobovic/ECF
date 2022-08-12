#include <cmath>
#include "../ECF_base.h"
#include "Permutation.h"
#include<sstream>


namespace Permutation
{

void Permutation::registerParameters(StateP state)
{
	registerParameter(state, "size", (voidP) new uint(1), ECF::UINT,
		"genotype size: number of indices (mandatory)");
}


bool Permutation::initialize (StateP state)
{
	voidP genp = getParameterValue(state, "size");
	size_ = *((uint*) genp.get());

	if(size_ < 1) {
		ECF_LOG_ERROR(state, "Error: 'size' must be > 0 for Permutation genotype!");
		throw("");
	}
	variables.resize(static_cast<int>(size_));

	for(uint i = 0; i < size_; i++) {
		variables[i] = (int) i;
	}
	int ind1, ind2, temp;

	// generate random permutation
	for(uint i = 0; i < size_; i++) {
		ind1 = state->getRandomizer()->getRandomInteger(size_);
		ind2 = state->getRandomizer()->getRandomInteger(size_);
		temp = variables[ind1];
		variables[ind1] = variables[ind2];
		variables[ind2] = temp;
	}
	return true;
}


void Permutation::write(XMLNode &xPermutation)
{
	xPermutation = XMLNode::createXMLTopNode("Permutation");
	std::stringstream sValue;
	sValue << size_;
	xPermutation.addAttribute("size", sValue.str().c_str());

	sValue.str("");
	for(uint i = 0; i < size_; i++)
		sValue << "\t" << variables[i];
	xPermutation.addText(sValue.str().c_str());
}


void Permutation::read(XMLNode &xPermutation)
{
	XMLCSTR xIndices = xPermutation.getText();
	std::stringstream sValues;
	sValues << xIndices;

	for(uint index = 0; index < size_; index++) {
		sValues >> variables[index];
	}
}

}