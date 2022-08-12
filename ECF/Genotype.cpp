#include "ECF_base.h"


bool Genotype::registerParameter(StateP state, std::string name, voidP value, enum ECF::type T, std::string description)
{
	return state->getRegistry()->registerEntry(name_ + "." + name, value, T, description, genotypeId_);
}


voidP Genotype::getParameterValue(StateP state, std::string name)
{
	return state->getRegistry()->getEntry(name_ + "." + name, genotypeId_);
}


bool Genotype::setParameterValue(StateP state, std::string name, voidP value)
{
	return state->getRegistry()->modifyEntry(name_ + "." + name, value, genotypeId_);
}


bool Genotype::isParameterDefined(StateP state, std::string name)
{
	return state->getRegistry()->isModified(name_ + "." + name, genotypeId_);
}