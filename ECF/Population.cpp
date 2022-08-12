#include "ECF_base.h"
#include <string.h>


Population::Population()
{
	hof_ = static_cast<HallOfFameP> (new HallOfFame);
	stats_ = static_cast<StatCalcP> (new StatCalc);
	
	nIndividuals_ = 100;
	nDemes_ = 1;
	myDemeIndex_ = 0;
}


void Population::registerParameters(StateP state)
{
	uint *sizep = new uint(100);
	state->getRegistry()->registerEntry("population.size", (voidP) sizep, ECF::UINT,
		"number of individuals (default: 100)");
	uint *demep = new uint(1);
	state->getRegistry()->registerEntry("population.demes", (voidP) demep, ECF::UINT,
		"number of demes (default: 1)");

	stats_->registerParameters(state);
}


#ifndef _MPI	// slijedna verzija


/**
 * \brief Initialize population (serial ECF).
 */
bool Population::initialize(StateP state)
{
	state_ = state;
	this->clear();

	hof_ = static_cast<HallOfFameP> (new HallOfFame);
	hof_->initialize(state);

	stats_ = static_cast<StatCalcP> (new StatCalc);
	stats_->initialize(state);

	voidP sptr = state->getRegistry()->getEntry("population.size");
	nIndividuals_ = *((uint*) sptr.get());
	sptr = state->getRegistry()->getEntry("population.demes");
	nDemes_ = *((uint*) sptr.get());

	for(uint i = 0; i < nDemes_; i++){
		this->push_back(static_cast<DemeP> (new Deme));
		this->back()->getSize() = nIndividuals_;
		this->back()->initialize(state);
	}

	return true;
}


/**
 * \brief Read population from XMLNode (serial ECF).
 * Population must be initialize()-d beforehand!
 */
void Population::read(XMLNode &xPopulation)
{
	XMLNode xHof = xPopulation.getChildNode(0);
	this->hof_->read(xHof);

	for(uint i = 0; i < this->size(); i++) {
		XMLNode xDeme = xPopulation.getChildNode((int)i + 1);
		this->at(i)->read(xDeme);
	}
}


/**
 * \brief Write population to XMLNode (serial ECF).
 */
void Population::write(XMLNode &xPopulation)
{
	xPopulation = XMLNode::createXMLTopNode(NODE_POPULATION);
	xPopulation.addAttribute("size", uint2str(nDemes_).c_str());	// number of demes

	XMLNode xHoF;
	hof_->write(xHoF);
	xPopulation.addChild(xHoF);

	for(uint iDeme = 0; iDeme < nDemes_; iDeme++) {
		XMLNode xDeme;
		this->at(iDeme)->write(xDeme);
		xPopulation.addChild(xDeme);
	}
}


/**
 * \brief Update, gather and write statistics of all demes; update and gather HallOfFame of all demes.
 */
void Population::updateDemeStats()
{	
	ECF_LOG(state_, 4, "Population: updating HoF and statistics of all demes");

	// operate statistics on all demes
	for(uint iDeme = 0; iDeme < this->size(); iDeme++) {
		DemeP deme = this->at(iDeme);
		deme->stats_->operate(*(deme));
		ECF_LOG(state_, 3, "Deme: " + uint2str(iDeme));
		deme->stats_->log();
	}

	// copy stats from first deme, update with others
	stats_->copyStats(this->at(0)->stats_);

	if(this->nDemes_ > 1) {
		for(uint iDeme = 1; iDeme < this->size(); iDeme++) 
			stats_->update(this->at(iDeme)->stats_->getStats());
		ECF_LOG(state_, 3, "Population:");
		stats_->log();
	}

	// gather HoF
	std::vector<IndividualP> pool;
	for(uint iDeme = 0; iDeme < this->size(); iDeme++) {
		this->at(iDeme)->hof_->operate(*(this->at(iDeme)));
		std::vector<IndividualP> bestOfDeme = this->at(iDeme)->hof_->getBest();
		for(uint i = 0; i < bestOfDeme.size(); i++)
			pool.push_back(bestOfDeme[i]);
		hof_->operate(pool);
	}
}


#else	// _MPI - paralelna verzija


/**
 * Initialize population (parallel ECF).
 */
bool Population::initialize(StateP state)
{
	this->clear();
	state_ = state;

	voidP sptr = state->getRegistry()->getEntry("population.size");
	nIndividuals_ = *((uint*) sptr.get());
	sptr = state->getRegistry()->getEntry("population.demes");
	nDemes_ = *((uint*) sptr.get());

	hof_ = static_cast<HallOfFameP> (new HallOfFame);
	hof_->initialize(state);

	stats_ = static_cast<StatCalcP> (new StatCalc);
	stats_->initialize(state);

	// single deme:
	if(nDemes_ == 1) {
		this->push_back(static_cast<DemeP> (new Deme));
		this->back()->getSize() = nIndividuals_;
		this->back()->initialize(state);
		myDemeIndex_ = 0;
		return true;
	}

	// multiple demes:
	myDemeIndex_ = state->getCommunicator()->createDemeCommunicator(nDemes_);
	this->push_back(static_cast<DemeP> (new Deme));
	this->back()->getSize() = nIndividuals_;
	this->back()->initialize(state);

	return true;
}


/**
 * Update, gather and write statistics of all demes; update and gather HallOfFame of all demes. (parallel ECF)
 */
void Population::updateDemeStats()
{
	ECF_LOG(state_, 4, "Population: updating HoF and statistics of all demes");
	// every process 'processes' its local deme
	ECF_LOG(state_, 3, "Deme: " + uint2str(getLocalDemeId()));
	this->at(0)->hof_->operate(*(getLocalDeme()));

	// operate hof and stats on local deme
	std::vector<IndividualP> bestPool = this->at(0)->hof_->getBest();
	hof_->operate(bestPool);

	this->at(0)->stats_->operate(*(getLocalDeme()));
	stats_->copyStats(this->at(0)->stats_);
	stats_->log();

	// update with remote demes
	if(nDemes_ > 1) {		// vise demova - svaki lokalni proces 0 dodje ovdje
		// globalni proces 0 prima hof-ove i statistiku od ostalih deme mastera
		if(state_->getCommunicator()->getCommGlobalRank() == 0) {

			std::vector<IndividualP> bestPool, received;
			bestPool = this->at(0)->hof_->getBest();	// uzmi moj hof za pocetak

			for(uint iDeme = 1; iDeme < nDemes_; iDeme++) {	// dodaj ostale
				received = state_->getCommunicator()->recvIndividualsGlobal();
				for(uint ind = 0; ind < received.size(); ind++)
					bestPool.push_back(received[ind]);
			}

			hof_->operate(bestPool);	// konacno, odredi globalni hof

			// gather statistics
			std::vector<double> statValues;
			for(uint iDeme = 1; iDeme < nDemes_; iDeme++) {
				statValues = state_->getCommunicator()->recvValuesGlobal();
				stats_->update(statValues);
			}
			ECF_LOG(state_, 3, "Population: ");
			stats_->log();
		}
		else {	// saljem globalnom 0
			// send HoF
			std::vector<IndividualP> myBest = this->at(0)->hof_->getBest();
			state_->getCommunicator()->sendIndividualsGlobal(myBest, 0);
			// send stats
			std::vector<double> myStats = stats_->getStats();
			state_->getCommunicator()->sendValuesGlobal(myStats, 0);
		}
	}
}


/**
 * Write population to XMLNode (parallel ECF).
 */
void Population::write(XMLNode &xPopulation)
{
	CommunicatorP comm = state_->getCommunicator();

	// only deme masters continue
	if(comm->getCommRank() != 0)
		return;

	xPopulation = XMLNode::createXMLTopNode(NODE_POPULATION);
	xPopulation.addAttribute("size", uint2str(nDemes_).c_str());

	XMLNode xHoF;
	hof_->write(xHoF);
	xPopulation.addChild(xHoF);

	// write local deme
	XMLNode xDeme;
	this->at(0)->write(xDeme);
	xPopulation.addChild(xDeme);

	// collect remote demes
	if(comm->getCommGlobalRank() == 0) {
		for(uint iDeme = 1; iDeme < nDemes_; iDeme++) {
			XMLNode xDeme;
			voidP msg = comm->recvDataGlobal();
			xDeme = XMLNode::parseString((char*) msg.get(), "Deme");
			xPopulation.addChild(xDeme);
		}
	}
	else {
		char* message = xDeme.createXMLString();
		comm->sendDataGlobal((voidP) message, (uint) strlen(message), 0);
	}
}


/**
 * Read population from XMLNode (parallel ECF).
 * Population must be initialize()-d beforehand!
 */
void Population::read(XMLNode &xPopulation)
{
	CommunicatorP comm = state_->getCommunicator();

	// only deme masters continue
	if(comm->getCommRank() != 0)
		return;

	XMLNode xHof = xPopulation.getChildNode(0);
	this->hof_->read(xHof);

	// read the designated deme
	XMLNode xDeme = xPopulation.getChildNode(getLocalDemeId() + 1);
	getLocalDeme()->read(xDeme);
}


#endif	// _MPI