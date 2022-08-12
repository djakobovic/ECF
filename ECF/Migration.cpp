#include "ECF_base.h"
#include "SelRandomOp.h"
#include "SelBestOp.h"
#include "SelWorstOp.h"

#include "Migration.h"


void Migration::registerParameters(StateP state)
{
	uint* freq = new uint(0);
	state->getRegistry()->registerEntry("migration.freq", (voidP) freq, ECF::UINT,
		"individuals are exchanged each 'freq' generations (default: none)");

	uint* num = new uint(1);
	state->getRegistry()->registerEntry("migration.number", (voidP) num, ECF::UINT,
		"number of individuals to be sent to another deme (default: 1)");
}


bool Migration::initialize(StateP state)
{
	state_ = state;

	voidP sptr = state->getRegistry()->getEntry("migration.freq");
	migFrequency_ = *((uint*) sptr.get());

	if(migFrequency_ == 0 && state->getPopulation()->getNoDemes() > 1) {
		ECF_LOG_ERROR(state, "Warning: migration operator is not configured (migration will not occur between demes).");
	}

	if(migFrequency_ == 0)
		return true;

	sptr = state->getRegistry()->getEntry("migration.number");
	nEmigrants_ = *((uint*) sptr.get());

	if(nEmigrants_ >= state->getPopulation()->getLocalDeme()->getSize()) {
		ECF_LOG_ERROR(state, "Error: number of emmigrants greater than deme size!");
		throw "";
	}

	if(state->getPopulation()->getNoDemes() == 1) {
		ECF_LOG_ERROR(state, "Warning: migration operator not applicable with a single deme population!");
		migFrequency_ = 0;
	}

	selOp_.push_back(static_cast<SelectionOperatorP> (new SelBestOp));
	selOp_.push_back(static_cast<SelectionOperatorP> (new SelRandomOp));

	selOp_[BEST]->initialize(state);
	selOp_[RANDOM]->initialize(state);

	return true;
}


#ifndef _MPI

bool Migration::operate(StateP state)
{
	if(migFrequency_ == 0 || state->getGenerationNo() % migFrequency_ != 0)
		return true;

	std::vector< std::vector<IndividualP> > outPool_, inPool_;
	outPool_.resize(state->getPopulation()->getNoDemes());
	inPool_.resize(state->getPopulation()->getNoDemes());

	//
	// emigrant selection: best + random
	// emigrant no: 'nEmigrants' parameter
	//
	for(uint iDeme = 0; iDeme < state->getPopulation()->getNoDemes(); iDeme++) {

		DemeP myDeme = state->getPopulation()->at(iDeme);
		std::vector<IndividualP> emigrants;
		IndividualP myBest = selOp_[BEST]->select(*myDeme);
		emigrants.push_back(myBest);
		for(uint i = 1; i < nEmigrants_; i++)
			emigrants.push_back(selOp_[RANDOM]->select(*myDeme));

		outPool_[iDeme] = emigrants;
	}

	// copy selected individuals
	for(uint i = 0; i < outPool_.size(); i++)
		for(uint j = 0; j < outPool_[i].size(); j++)
			outPool_[i][j] = (IndividualP) outPool_[i][j]->copy();

	//
	// topology: ring
	//
	for(uint iDeme = 0; iDeme < state->getPopulation()->getNoDemes(); iDeme++) {

		uint destDeme = (iDeme + 1) % state->getPopulation()->getNoDemes();
		inPool_[destDeme] = outPool_[iDeme];
	}

	//
	// replacement: random (protect the best)
	//
	for(uint iDeme = 0; iDeme < state->getPopulation()->getNoDemes(); iDeme++) {

		DemeP myDeme = state->getPopulation()->at(iDeme);
		IndividualP myBest = selOp_[BEST]->select(*myDeme);
		std::vector<IndividualP> immigrants = inPool_[iDeme];
		ECF_LOG(state, 4, "Received inds fitness: " + dbl2str(immigrants[0]->fitness->getValue()));

		for(int i = (int) immigrants.size() - 1; i >= 0; i--) {
			IndividualP victim;
			do
				victim = selOp_[RANDOM]->select(*myDeme);
			while(victim == myBest);

			state->getAlgorithm()->replaceWith(victim, immigrants[i]);
		}
	}

	return true;
}


#else // _MPI

bool Migration::operate(StateP state)
{
	if(migFrequency_ == 0 || state->getGenerationNo() % migFrequency_ != 0)
		return true;

	CommunicatorP comm = state->getCommunicator();

	//
	// emigrant selection: best + random
	// emigrant no: 'nEmigrants' parameter
	//
	DemeP myDeme = state->getPopulation()->at(0);
	std::vector<IndividualP> emigrants;
	IndividualP myBest = selOp_[BEST]->select(*myDeme);
	emigrants.push_back(myBest);
	for(uint i = 1; i < nEmigrants_; i++)
		emigrants.push_back(selOp_[RANDOM]->select(*myDeme));

	//
	// topology: ring
	//
	uint destDeme = (state->getPopulation()->getLocalDemeId() + 1) % state->getPopulation()->getNoDemes();
	uint destProcess = comm->getDemeMaster(destDeme);
	comm->sendIndividualsGlobal(emigrants, destProcess);

	//
	// replacement: random (protect the best)
	//
	std::vector<IndividualP> immigrants = comm->recvIndividualsGlobal();
	ECF_LOG(state, 4, "Received inds fitness: " + dbl2str(immigrants[0]->fitness->getValue()));

	for(int i = (int) immigrants.size() - 1; i >= 0; i--) {
		IndividualP victim;
		do
			victim = selOp_[RANDOM]->select(*myDeme);
		while(victim == myBest);

		state->getAlgorithm()->replaceWith(victim, immigrants[i]);
	}

	return true;
}

#endif // _MPI