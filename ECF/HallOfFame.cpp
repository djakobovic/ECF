#include "ECF_base.h"
#include "HallOfFame.h"
#include <string>
#include <sstream>


HallOfFame::HallOfFame() 
{
	this->selectBest_ = static_cast<SelBestOpP> (new SelBestOp);
	hofSize_ = 1;
	bEmpty_ = true;
	lastChangeGen_ = 0;
}


bool HallOfFame::initialize(StateP state)
{
	state_ = state;
	bestIndividuals_.clear();
	bestIndividuals_.resize(hofSize_);
	bestGenerations_.clear();
	bestGenerations_.resize(hofSize_);
	bEmpty_ = true;
	lastChangeGen_ = 0;

	return true;
}


/**
 * \brief Collect best individuals of the whole Population
 */
bool HallOfFame::operate(StateP state)
{
	PopulationP population = state->getPopulation();

	// iterate for all demes
	for(uint i = 0; i < population->size(); i++) {
		operate(*(population->at(i)));
	}
	return true;
}


/**
 * \brief Collect best individuals of the given vector
 */
bool HallOfFame::operate(const std::vector<IndividualP>& individuals)
{
	uint ind = 0;
	IndividualP best;

	while (ind < hofSize_) {
		best = selectBest_->select(individuals);

		// TODO: reimplement for hofSize_ > 1
		if(bEmpty_ || best->fitness->isBetterThan(bestIndividuals_[ind]->fitness)) {	
			// copy individual to HoF
			bestIndividuals_[ind] = (IndividualP) best->copy();
			bestGenerations_[ind] = state_->getGenerationNo();
			bEmpty_ = false;
			lastChangeGen_ = state_->getGenerationNo();
		}
		++ind;
	}

	return true;
}


void HallOfFame::write(XMLNode& xHoF)
{
	xHoF = XMLNode::createXMLTopNode("HallOfFame");
	std::stringstream sValue;
	sValue << this->hofSize_;
	xHoF.addAttribute("size", uint2str(hofSize_).c_str());

	XMLNode xInd;
	for(uint i = 0; i < hofSize_; i++) {
		bestIndividuals_[i]->write(xInd);
		xInd.addAttribute("gen", uint2str(bestGenerations_[i]).c_str());
		xHoF.addChild(xInd);
	}
}


void HallOfFame::read(XMLNode& xHof)
{
	std::stringstream ss;
	ss << xHof.getAttribute("size");
	ss >> hofSize_;
	ss.clear(); ss.str("");

	bestIndividuals_.resize(hofSize_);
	bestGenerations_.resize(hofSize_);
	bEmpty_ = false;
	lastChangeGen_ = 0;

	XMLNode xInd;
	for(uint i = 0; i < hofSize_; i++) {
		xInd = xHof.getChildNode((int)i);
		ss << xInd.getAttribute("gen");
		ss >> bestGenerations_[i];
		ss.clear(); ss.str("");
		bestIndividuals_[i] = (IndividualP) state_->getIndividualObject()->copy();
		bestIndividuals_[i]->read(xInd);

		if(lastChangeGen_ < bestGenerations_[i])
			lastChangeGen_ = bestGenerations_[i];
	}
}
