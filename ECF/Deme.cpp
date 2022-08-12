#include "ECF_base.h"
#include<sstream>


bool Deme::initialize(StateP state)
{
	for(uint i = 0; i < nIndividuals_; i++) {
		this->push_back(static_cast<IndividualP> (new Individual));
		this->back()->index = (unsigned int) this->size() - 1;
		this->back()->initialize(state);
	}

	hof_ = static_cast<HallOfFameP> (new HallOfFame);
	hof_->initialize(state);

	stats_ = static_cast<StatCalcP> (new StatCalc);
	stats_->initialize(state);

	return true;
}


bool Deme::replace(uint index, IndividualP newInd)
{
	newInd->index = index;
	(*this)[index] = newInd;
	return true;
}


void Deme::write(XMLNode &xDeme)
{
	xDeme = XMLNode::createXMLTopNode("Deme");
	std::stringstream value;
	value << nIndividuals_;
	xDeme.addAttribute("size", value.str().c_str());

	XMLNode xHoF;
	hof_->write(xHoF);
	xDeme.addChild(xHoF);

	XMLNode xIndividual;
	for(uint i = 0; i < this->size(); i++) {
		this->at(i)->write(xIndividual);
		xDeme.addChild(xIndividual);
	}
}


/**
 * Read deme from XMLNode.
 * Population must be initialize()-d beforehand!
 */
void Deme::read(XMLNode &xDeme)
{
	XMLNode xHof = xDeme.getChildNode(0);
	this->hof_->read(xHof);

	for(uint i = 0; i < this->size(); i++) {
		XMLNode xInd = xDeme.getChildNode((int)i + 1);
		this->at(i)->read(xInd);
	}
}