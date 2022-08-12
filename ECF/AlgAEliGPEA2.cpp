#include "ECF_base.h"
#include "ECF_derived.h"
#include "ECF_macro.h"

#include "AlgAEliGPEA2.h"

const int MASTER = 0;
const int RANDOM = 0;
const int WORST = 1;

AlgAEliGpea2::AlgAEliGpea2()
{
	name_ = "AlgAEliGPEA2";
	selectionOp.push_back(static_cast<SelectionOperatorP> (new SelRandomOp));
	selectionOp.push_back(static_cast<SelectionOperatorP> (new SelWorstOp));
}

void AlgAEliGpea2::registerParameters(StateP state)
{
	int* tsizep = new int(3);
	state->getRegistry()->registerEntry(name_ + ".tsize", (voidP) tsizep, ECF::UINT);

	uint* jobSize = new uint(10);
	state->getRegistry()->registerEntry(name_ + ".jobsize", (voidP) jobSize, ECF::UINT);
}

bool AlgAEliGpea2::initialize(StateP state)
{
	if(state->getCommunicator()->getCommRank() != MASTER)
		myJob.resize(0);

    selectionOp[RANDOM]->initialize(state);
    selectionOp[WORST]->initialize(state);

	voidP tsizep = state->getRegistry()->getEntry(name_ + ".tsize");
    nTournament = *((uint*) tsizep.get());

	voidP jobSizeP = state->getRegistry()->getEntry(name_ + ".jobsize");
	jobSize = *((uint*) jobSizeP.get());

    return true;
}

bool AlgAEliGpea2::advanceGeneration(StateP state, DemeP deme)
{
	CommunicatorP comm = state->getCommunicator();

	if(comm->getCommRank() == MASTER) {

		std::vector<IndividualP> evalPool;
		uint iIter = 0;

		while(iIter < deme->size()) {

			// perform jobSize selections and operations
			for(uint ind = 0; ind < jobSize && iIter < deme->size(); ind++, iIter++) {
				std::vector<IndividualP> tournament;
				for (uint i = 0; i < nTournament; ++i) {
					tournament.push_back(selectionOp[RANDOM]->select(*deme));
				}

				IndividualP worst = selectionOp[WORST]->select(tournament);
				removeFrom(worst, tournament);	// remove pointer 'worst' from vector 'tournament'

				crossover_->mate(tournament[0], tournament[1], worst);

				std::vector<IndividualP> mutationPool;
				mutationPool.push_back(worst);
				mutation_->mutation(mutationPool);

				if(isMember(worst, evalPool) == false)
					evalPool.push_back(worst);
			}

			// if a worker is ready, send individuals to evaluate
			//if(comm->messageWaiting(MPI::ANY_SOURCE))

			// OR, just wait for a worker to be ready
				comm->recvDemeFitness(*deme, Comm::ANY_PROCESS);
				uint iWorker = comm->getLastSource();

				comm->sendIndividuals(evalPool, iWorker);
				evalPool.resize(0);
		}

		// this is redundant IF the MASTER waits for a WORKER (above)
		//std::vector<IndividualP> job;
		//int remaining = (int) evalPool.size() - 1;
		//while(remaining >= 0) {
		//	comm->recvDemeFitness(*deme, MPI::ANY_SOURCE);
		//	uint iWorker = comm->getLastSource();

		//	job.resize(0);
		//	int iInd;
		//	for(iInd = remaining; iInd >= 0 && remaining - iInd < (int) jobSize; iInd--)
		//		job.push_back(evalPool[iInd]);
		//	remaining = iInd;
		//	comm->sendIndividuals(job, iWorker);
		//}
		//evalPool.resize(0);

		// while all individuals not evaluated
		uint remainingWorkers = comm->getCommSize() - 1;
		while(remainingWorkers > 0) {
			comm->recvDemeFitness(*deme, Comm::ANY_PROCESS);
			uint iWorker = comm->getLastSource();

			comm->sendIndividuals(evalPool, iWorker);
			remainingWorkers--;
		}
	}

	else {	// WORKER
		std::vector<IndividualP> empty;
		empty.resize(0);
		comm->sendFitness(empty, MASTER);

		uint myJobSize;
		myJobSize = comm->recvReplaceIndividuals(myJob, MASTER);

		// while individuals to evaluate
		while(myJobSize > 0) {
			for(uint i = 0; i < myJobSize; i++)
				evaluate(myJob[i]);

			comm->sendFitness(myJob, MASTER, myJobSize);

			myJobSize = comm->recvReplaceIndividuals(myJob, MASTER);
		}
	}

	return true;
}
