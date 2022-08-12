#include "ECF_base.h"
#include<sstream>


Individual::Individual(StateP state)
{
	cid = 0;
	initialize(state);
}


bool Individual::initialize(StateP state)
{
	state_ = state;
	this->clear();

	// copy genotypes from State
	for(uint i = 0; i < state->getGenotypes().size(); i++) {
		this->push_back(static_cast<GenotypeP> (state->getGenotypes()[i]->copy()));
		(*this)[i]->setGenotypeId(i);
	}

	// init genotypes
	for(uint i = 0; i < this->size(); i++)
		(*this)[i]->initialize(state);

	return true;
}


Individual* Individual::copy()
{
	Individual *c = new Individual;
	c->state_ = state_;
	c->cid = cid;
	if(fitness)
		c->fitness = (FitnessP) this->fitness->copy();
	for(uint i = 0; i < this->size(); i++)
		c->push_back((GenotypeP) (this->at(i)->copy()));
	return c;
}


GenotypeP Individual::getGenotype(uint index)
{
	return this->at(index);
}


FitnessP Individual::getFitness()
{
	return fitness;
}


/**
 * Write individual to XMLNode.
 *
 * Individual must be initialize()-d beforehand!
 * If fitness object doesn't exist, an empty Fitness node is created.
 */
void Individual::write(XMLNode &xIndividual)
{
	xIndividual = XMLNode::createXMLTopNode("Individual");
	std::stringstream sValue;
	sValue << this->size();
	xIndividual.addAttribute("size", sValue.str().c_str());

	if (fitness) {
		XMLNode xFitness;
		fitness->write(xFitness);
		xIndividual.addChild(xFitness);
	} else {
		XMLNode xFitness = XMLNode::createXMLTopNode("Fitness");
		xIndividual.addChild(xFitness);
	}

	XMLNode xGenotype;
	for(uint i = 0; i < this->size(); i++) {
		this->at(i)->write(xGenotype);
		xIndividual.addChild(xGenotype);
	}
}


std::string Individual::toString()
{
	XMLNode xInd;
	write(xInd);
	char *s = xInd.createXMLString();
	std::string out(s);
	freeXMLString(s);
	return out;
}


/**
 * Read individual from XMLNode.
 *
 * Individual must be initialize()-d beforehand!
 * If fitness object doesn't exist, one is copied from State.
 */
void Individual::read(XMLNode &xIndividual)
{
	XMLNode xFit = xIndividual.getChildNode(0);

	if(!fitness)
		fitness = static_cast<FitnessP> (state_->getFitnessObject()->copy());
	this->fitness->read(xFit);

	for(uint i = 0; i < this->size(); i++) {
		XMLNode xGen = xIndividual.getChildNode((int)i + 1);
		this->at(i)->read(xGen);
	}
}