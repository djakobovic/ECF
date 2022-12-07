#include <cmath>
#include <sstream>
#include "../ECF_base.h"
#include "IntGenotype.h"

namespace IntGenotype
{
	
	void IntGenotype::registerParameters(StateP state)
	{
		registerParameter(state, "lbound", (voidP) new int(-10), ECF::INT);
		registerParameter(state, "ubound", (voidP) new int(10), ECF::INT);
		registerParameter(state, "size", (voidP)(new uint(1)), ECF::UINT);
	}

	bool IntGenotype::initialize(StateP state)
	{
		if (!isParameterDefined(state, "lbound") ||
			!isParameterDefined(state, "ubound") ||
			!isParameterDefined(state, "size")) {
			ECF_LOG_ERROR(state, "Error: required parameters for IntGenotype genotype not defined (lbound, ubound, size)!");
			throw("");
	}

		voidP genp = getParameterValue(state, "lbound");
		minValue_ = *((int*)genp.get());

		genp = getParameterValue(state, "ubound");
		maxValue_ = *((int*)genp.get());

		if (minValue_ >= maxValue_) {
			ECF_LOG_ERROR(state, "Error: 'lbound' must be smaller than 'ubound' for IntGenotype genotype!");
			throw("");
		}

		genp = getParameterValue(state, "size");
		nSize_ = *((uint*)genp.get());

		if (nSize_ < 1) {
			ECF_LOG_ERROR(state, "Error: 'size' must be > 0 for IntGenotype genotype!");
			throw("");
		}

		intValues.resize(nSize_);
		// randomly create each dimension
		for (uint i = 0; i < nSize_; i++){
			intValues[i] = state->getRandomizer()->getRandomInteger(minValue_, maxValue_);
		}

		return true;
	}

	void IntGenotype::write(XMLNode &xIntGenotype)
	{
		xIntGenotype = XMLNode::createXMLTopNode("IntGenotype");
		std::stringstream sValue;
		sValue << intValues.size();
		xIntGenotype.addAttribute("size", sValue.str().c_str());

		sValue.str("");
		for (uint iVar = 0; iVar < intValues.size(); iVar++)
			sValue << " " << intValues[iVar];
		xIntGenotype.addText(sValue.str().c_str());
	}

	// mandatory if running parallel ECF or reading population from a milestone file
	void IntGenotype::read(XMLNode& xIntGenotype)
	{
		XMLCSTR values = xIntGenotype.getText();
		std::stringstream sValues;
		sValues << values;

		for (uint iVar = 0; iVar < intValues.size(); iVar++)
			sValues >> intValues[iVar];
	}
	
}