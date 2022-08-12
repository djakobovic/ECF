#include <cmath>
#include "../ECF_base.h"
#include "FloatingPoint.h"
#include<sstream>

namespace FloatingPoint
{

void FloatingPoint::registerParameters(StateP state)
{
	registerParameter(state, "lbound", (voidP) new double(-10), ECF::DOUBLE,
		"lower bound for each variable (mandatory)");
	registerParameter(state, "ubound", (voidP) new double(10), ECF::DOUBLE,
		"upper bound for each variable (mandatory)");
	registerParameter(state, "dimension", (voidP) new uint(1), ECF::UINT,
		"number of real valued variables (mandatory)");
}
 

bool FloatingPoint::initialize (StateP state)
{
	if(!isParameterDefined(state, "lbound") ||
		!isParameterDefined(state, "ubound") ||
		!isParameterDefined(state, "dimension")) {
			ECF_LOG_ERROR(state, "Error: required parameters for FloatingPoint genotype not defined (lbound, ubound, dimension)!");
			throw("");
	}

	voidP genp = getParameterValue(state, "lbound");
	minValue_ = *((double*) genp.get());

	genp = getParameterValue(state, "ubound");
	maxValue_ = *((double*) genp.get());

	if(minValue_ >= maxValue_) {
		ECF_LOG_ERROR(state, "Error: 'lbound' must be smaller than 'ubound' for FloatingPoint genotype!");
		throw("");
	}

	genp = getParameterValue(state, "dimension");
	nDimension_ = *((uint*) genp.get());

	if(nDimension_ < 1) {
		ECF_LOG_ERROR(state, "Error: 'dimension' must be > 0 for FloatingPoint genotype!");
		throw("");
	}

	realValue.resize(nDimension_);
	// randomly create each dimension
	for (uint i = 0; i < nDimension_; i++){
		realValue[i] = ( minValue_ + (maxValue_ - minValue_) * state->getRandomizer()->getRandomDouble() );
	}

	return true;
}


void FloatingPoint::write(XMLNode &xFloatingPoint)
{
	xFloatingPoint = XMLNode::createXMLTopNode("FloatingPoint");
	std::stringstream sValue;
	sValue << nDimension_;
	xFloatingPoint.addAttribute("size", sValue.str().c_str());

	sValue.str("");
	for(uint iVar = 0; iVar < nDimension_; iVar++)
		sValue << "\t" << realValue[iVar];
	xFloatingPoint.addText(sValue.str().c_str());
}


void FloatingPoint::read(XMLNode& xFloatingPoint)
{
	XMLCSTR values = xFloatingPoint.getText();
	std::stringstream sValues;
	sValues << values;

	for(uint iVar = 0; iVar < nDimension_; iVar++) {
		sValues >> realValue[iVar];
	}
}

}