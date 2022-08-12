#include "ECF_base.h"
#include "ECF_derived.h"
#include "ECF_macro.h"

#include "AlgAEliGPEA.h"

const int MASTER = 0;
const int RANDOM = 0;
const int WORST = 1;


AlgAEliGpea::AlgAEliGpea()
{
	name_ = "AlgAEliGPEA";
	selectionOp.push_back(static_cast<SelectionOperatorP> (new SelRandomOp));
	selectionOp.push_back(static_cast<SelectionOperatorP> (new SelWorstOp));
}


void AlgAEliGpea::registerParameters(StateP state)
{
	int* tsizep = new int(3);
	state->getRegistry()->registerEntry(name_ + ".tsize", (voidP) tsizep, ECF::UINT);

	uint* jobSize = new uint(10);
	state->getRegistry()->registerEntry(name_ + ".jobsize", (voidP) jobSize, ECF::UINT);
}


bool AlgAEliGpea::initialize(StateP state)
{
	if(state->getCommunicator()->getCommRank() != MASTER)
		myJob_.resize(0);

    selectionOp[RANDOM]->initialize(state);
    selectionOp[WORST]->initialize(state);

	voidP tsizep = state->getRegistry()->getEntry(name_ + ".tsize");
    nTournament_ = *((uint*) tsizep.get());

	if(nTournament_ < 3) {
		ECF_LOG_ERROR(state, "Error: AlgAEliGPEA algorithm requires minimum tournament size of 3!");
		throw "";
	}

	voidP jobSizeP = state->getRegistry()->getEntry(name_ + ".jobsize");
	jobSize_ = *((uint*) jobSizeP.get());

    return true;
}


IndividualP AlgAEliGpea::performSingleTournament(DemeP deme)
{
	std::vector<IndividualP> tournament;
	for (uint i = 0; i < nTournament_; ++i) {
		tournament.push_back(selectionOp[RANDOM]->select(*deme));
	}

	IndividualP worst = selectionOp[WORST]->select(tournament);
	storeIndividual(worst);

	removeFrom(worst, tournament);	// remove pointer 'worst' from vector 'tournament'

	mate(tournament[0], tournament[1], worst);

	mutate(worst);

	return worst;
}


bool AlgAEliGpea::advanceGeneration(StateP state, DemeP deme)
{
	CommunicatorP comm = state->getCommunicator();

	if(comm->getCommRank() == MASTER) {

		std::vector<IndividualP> evalPool;
		uint iIter = 0;

		while(iIter < deme->size()) {

			// perform jobSize selections and operations
			for(uint ind = 0; ind < jobSize_ && iIter < deme->size(); ind++, iIter++) {
				IndividualP newInd = performSingleTournament(deme);

				if(isMember(newInd, evalPool) == false)
					evalPool.push_back(newInd);
			}

			// evaluation-waiting loop
			do {
				// if a worker is ready
				if(!evalPool.empty() && comm->messageWaiting()) {
					std::vector<uint> received = comm->recvFitnessVector(*deme, Comm::ANY_PROCESS);
					state->increaseEvaluations((uint) received.size());
					uint iWorker = comm->getLastSource();

					// send job
					storeGenotypes(evalPool);
					comm->sendIndividuals(evalPool, iWorker);
					evalPool.resize(0);

					restoreIndividuals(received);
					break;	// continue main loop
				}
				// do something useful in the meantime...
				else {
					evaluate(evalPool.back());
					setConsistency(evalPool.back());
					evalPool.pop_back();

					IndividualP newInd = performSingleTournament(deme);
					iIter++;
					if(isMember(newInd, evalPool) == false)
						evalPool.push_back(newInd);
				}
			} while(iIter < deme->size());
		}

	}

	// WORKER
	else {
		myJob_.resize(0);
		comm->sendFitness(myJob_, MASTER);
		uint myJobSize = 1;

		// while individuals to evaluate
		while(myJobSize != 0) {

			myJobSize = comm->recvReplaceIndividuals(myJob_, MASTER);
			for(uint i = 0; i < myJobSize; i++)
				evaluate(myJob_[i]);

			comm->sendFitness(myJob_, MASTER, myJobSize);
		}
		ECF_LOG(state, 4, "Worker ends...");
	}

	return true;
}


void AlgAEliGpea::bcastTermination(StateP state)
{
	// master: if terminate condition is set, send all workers empty jobs
	if(state->getCommunicator()->getCommRank() == 0 && state->getTerminateCond()) {
		std::vector<IndividualP> empty;
		DemeP myDeme = state->getPopulation()->getLocalDeme();

		for(uint iWorker = 1; iWorker < state->getCommunicator()->getCommSize(); iWorker++) {
			if(state->getCommunicator()->messageWaiting(iWorker)) {
				std::vector<uint> received = state->getCommunicator()->recvFitnessVector(*myDeme, Comm::ANY_PROCESS);
				restoreIndividuals(received);
			}
			state->getCommunicator()->sendIndividuals(empty, iWorker);
		}

		// restore individuals to last consistent state
		restorePopulation();
	}

	// worker: evolution is over
	else if(state->getCommunicator()->getCommRank() != 0)
		state->setTerminateCond();
}