#include "ECF_base.h"
#include "MOFitness.h"


bool MOFitness::isBetterThan(FitnessP other)
{
	MOFitnessP p = boost::static_pointer_cast<MOFitness> (other);
	uint objCount = (uint) p->size();

	for (uint i = 0; i<objCount; i++) {
		if ( this->rank < p->rank) {
			return true;
		}
		if (p->rank < this->rank) {
			return false;
		}
		if (this->crowding_distance > p->crowding_distance) {
			return true;
		} 
		return false;

	} 

	return true;
}

MOFitness* MOFitness::copy()
{
	MOFitness *newObject = new MOFitness(*this);
	// copy individual Fitness objects from the original
	for(int i = 0; i < (int) this->size(); i++) {
		(*newObject)[i] = (FitnessP) ((*this)[i]->copy());
	}
	newObject->rank = this->rank;
	newObject->nc = this->nc;
	newObject->Sp = this->Sp;
	newObject->crowding_distance = this->crowding_distance; // ako se kopija ukljuci u populaciju crowding_distance ce biti 0 za obje jedinke

	return newObject;
}

void MOFitness::write(XMLNode &xFitness)
{
	xFitness = XMLNode::createXMLTopNode("MOFitness");
	uint size = (uint) this->size();
	for(uint i = 0; i < size; i++) {
		std::stringstream sValue;
		sValue << this->at(i)->getValue();
		std::string value = "value" + uint2str(i + 1);
		xFitness.addAttribute(value.c_str(), sValue.str().c_str());
	}
}

double MOFitness::getValue()
{	
	return this->rank;
}

double MOFitness::getValueOfObjective(int objective)
{
	return this->at(objective)->getValue();	
}

double MOFitness::getProperty(std::string prop, int objective = -1) {
	if (prop.compare("objective") == 0) {
		if (objective >= 0 && objective < (int) this->size()) {
			return getValueOfObjective(objective);
		} else {
			// zatrazena funkcija cilja ne postoji
			throw "";
		}
	} else if (prop.compare("rank") == 0) {
		return (double) this->rank;
	} else if (prop.compare("crowding_distance") == 0) {
		return this->crowding_distance;
	} else {
		// zatrazeni property ne postoji
		throw "";
	}
}