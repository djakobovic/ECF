#include <cmath>
#include "../ECF_base.h"
#include "Binary.h"
#include<sstream>

namespace Binary
{

double Binary::logbase (double a, double base) 
{
	return log(a) / log(base);
}


double Binary::round(double val, int decimals)
{
	double r = val * pow(10., decimals);
	r = floor(r + 0.5);
	return r / pow(10., decimals);
}


void Binary::registerParameters(StateP state)
{
	registerParameter(state, "lbound", (voidP) new double(0), ECF::DOUBLE, 
		"lower bound for each variable (mandatory)");
	registerParameter(state, "ubound", (voidP) new double(10), ECF::DOUBLE,
		"upper bound for each variable (mandatory)");
	registerParameter(state, "precision", (voidP) new uint(1), ECF::UINT,
		"number of digits after the decimal point (mandatory)");
	registerParameter(state, "dimension", (voidP) new uint(1), ECF::UINT,
		"number of real valued variables (mandatory)");
	registerParameter(state, "rounding", (voidP) new uint(0), ECF::UINT,
		"should the real value be rounded to the 'precision'-th decimal (default: no)");
}


bool Binary::initialize (StateP state)
{
	if(!isParameterDefined(state, "lbound") ||
		!isParameterDefined(state, "ubound") ||
		!isParameterDefined(state, "precision") ||
		!isParameterDefined(state, "dimension")) {
			ECF_LOG_ERROR(state, "Error: required parameters for Binary genotype not defined (lbound, ubound, precision, dimension)!");
			throw("");
	}

	voidP genp = getParameterValue(state, "lbound");
	minValue_ = *((double*) genp.get());

	genp = getParameterValue(state, "ubound");
	maxValue_ = *((double*) genp.get());

	if(minValue_ >= maxValue_) {
		ECF_LOG_ERROR(state, "Error: 'lbound' must be smaller than 'ubound' for Binary genotype!");
		throw("");
	}

	genp = getParameterValue(state, "precision");
	nDecimal_ = *((uint*) genp.get());

	if(nDecimal_ > 16) {
		ECF_LOG_ERROR(state, "Error: 'precision' too large (> 16) for Binary genotype!");
		throw("");
	}

	genp = getParameterValue(state, "dimension");
	nDimension_ = *((uint*) genp.get());

	if(nDimension_ < 1) {
		ECF_LOG_ERROR(state, "Error: 'dimension' must be > 0 for Binary genotype!");
		throw("");
	}

	genp = getParameterValue(state, "rounding");
	bRounding_ = (*((uint*) genp.get())) == 0 ? false : true;

	double numIndividual = ((maxValue_ - minValue_) * pow(10., (int) nDecimal_));
	nBits_ = static_cast<int> (logbase(numIndividual, 2) + 1);

	variables.resize(nDimension_);
	decValue.resize(nDimension_);
	realValue.resize(nDimension_);
	vBool_.resize(nBits_); 

	potention_ = static_cast<long> (pow(2., (int) nBits_) - 1);

	// randomly create each dimension
	for (uint i = 0; i < nDimension_; i++){
		realValue[i] = ( minValue_ + (maxValue_ - minValue_) * state->getRandomizer()->getRandomDouble() );
		decValue[i] = static_cast<long int> ((realValue[i] - minValue_) / (maxValue_ - minValue_) * potention_);
		if(bRounding_) {
			realValue[i] = round(realValue[i], (int)nDecimal_);
		}

		long dec = decValue[i];
		for (uint iBit = nBits_; iBit > 0; dec = dec / 2, iBit--) {
			// the bit with index 0 is the most significant bit
			vBool_[iBit - 1] = (dec % 2) ? true:false;
		}
		variables[i] = vBool_;
	}

	return true;
}


bool Binary::update (void)
{
	for (uint iDim = 0; iDim < nDimension_; iDim++) {
		long dec = 0;
		long weight = 1;
		for (uint iBit = nBits_ - 1; iBit >= 0; iBit--) {
			dec += static_cast<int>(variables[iDim][iBit]) * weight;
			weight *= 2;
		}

		decValue[iDim] = dec;
		realValue[iDim] = minValue_ + (maxValue_ - minValue_) / potention_ * dec;
		if(bRounding_) {
			realValue[iDim] = round(realValue[iDim], (int)nDecimal_);
		}
	}

	return true;
}


void Binary::write(XMLNode &xBinary)
{
	xBinary = XMLNode::createXMLTopNode("Binary");
	std::stringstream sValue;
	sValue << nDimension_;
	xBinary.addAttribute("size", sValue.str().c_str());

	sValue.str("");
	for(uint iVar = 0; iVar < nDimension_; iVar++)
		sValue << "\t" << realValue[iVar];
	xBinary.addText(sValue.str().c_str());
}


void Binary::read(XMLNode& xBinary)
{
	XMLCSTR values = xBinary.getText();
	std::stringstream sValues;
	sValues << values;

	for(uint iVar = 0; iVar < nDimension_; iVar++) {
		sValues >> realValue[iVar];
		decValue[iVar] = static_cast<long int> ((realValue[iVar] - minValue_) / (maxValue_ - minValue_) * potention_);

		long dec = decValue[iVar];
		for (uint iBit = nBits_; iBit > 0; dec = dec/2, iBit--) {
			vBool_[iBit - 1] = (dec % 2) ? true:false;
		}
		variables[iVar] = vBool_;
	}

}

}