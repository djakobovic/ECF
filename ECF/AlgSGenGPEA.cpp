#include "ECF_base.h"
#include "ECF_derived.h"
#include "ECF_macro.h"

#include "AlgSGenGPEA.h"

const int MASTER = 0;


AlgSGenGpea::AlgSGenGpea()
{
	name_ = "AlgSGenGPEA";
	selFitPropOp = static_cast<SelFitnessProportionalOpP> (new SelFitnessProportionalOp);
	selRandomOp = static_cast<SelRandomOpP> (new SelRandomOp);
	selBestOp = static_cast<SelBestOpP> (new SelBestOp);
}


void AlgSGenGpea::registerParameters(StateP state)
{
	double* crxRate = new double(0.5);
    state->getRegistry()->registerEntry(name_ + ".crxprob", (voidP) crxRate, ECF::DOUBLE);

	double* selPress = new double(2);
	state->getRegistry()->registerEntry(name_ + ".selpressure", (voidP) selPress, ECF::DOUBLE);

	uint* jobSize = new uint(10);
	state->getRegistry()->registerEntry(name_ + ".jobsize", (voidP) jobSize, ECF::UINT);
}


bool AlgSGenGpea::initialize(StateP state)
{
    selFitPropOp->initialize(state);
	selRandomOp->initialize(state);
	selBestOp->initialize(state);

	voidP crRateP = state->getRegistry()->getEntry(name_ + ".crxprob");
    crxRate = *((double*) crRateP.get());

	voidP selPressP = state->getRegistry()->getEntry(name_ + ".selpressure");
	selPressure = *((double*) selPressP.get());
	selFitPropOp->setSelPressure(selPressure);

	voidP jobSizeP = state->getRegistry()->getEntry(name_ + ".jobsize");
	jobSize = *((uint*) jobSizeP.get());

	if(state->getCommunicator()->getCommRank() != MASTER) {
		myJob.resize(0);
	}

    return true;
}


bool AlgSGenGpea::advanceGeneration(StateP state, DemeP deme)
{
	CommunicatorP comm = state->getCommunicator();

	if(comm->getCommRank() == MASTER) {

		// elitism
		IndividualP best = selBestOp->select(*deme);
		best = copy(best);

		std::vector<IndividualP> wheel;
		wheel = selFitPropOp->selectMany(*deme, (uint) deme->size());

		for(uint i = 0; i < wheel.size(); ++i) 
			wheel[i] = copy(wheel[i]);

		// replace population
		for(uint i = 0; i < deme->size(); i++)
			replaceWith(deme->at(i), wheel[i]);

		ECF_LOG(state, 5, "Selected individuals:");
		for(uint i = 0; i < deme->size(); i++){
			ECF_LOG(state, 5, dbl2str(deme->at(i)->fitness->getValue()));
		}

		uint noCrx = (int)(deme->size() * crxRate /2);

		for(uint i = 0; i < noCrx; i++){
			IndividualP parent1 = selRandomOp->select(*deme);
			IndividualP parent2 = selRandomOp->select(*deme);
			ECF_LOG(state, 5, "Parents: " + dbl2str(parent1->fitness->getValue()) + ", " + dbl2str(parent2->fitness->getValue()));
			IndividualP child1 = copy(parent1);
			IndividualP child2 = copy(parent2);
			mate(parent1, parent2, child1);
			mate(parent1, parent2, child2);
			replaceWith(parent1, child1);
			replaceWith(parent2, child2);
		}

		// perform mutation on whole population
		mutate(*deme);

		// prepare vector of individuals to evaluate
		std::vector<IndividualP> pool, job;
		for(uint i = 0; i < deme->size(); i++)
			if(!deme->at(i)->fitness->isValid()) 
				pool.push_back(deme->at(i));

		// while all individuals not sent
		uint current = 0, remaining = (uint) pool.size();
		while(current < pool.size()) {
			// if a worker is ready, receive and send new job
			if(comm->messageWaiting()) {
				// receive fitness objects, increase total number of evaluations
				uint received = comm->recvDemeFitness(*deme, Comm::ANY_PROCESS);
				state->increaseEvaluations(received);
				remaining -= received;

				uint iWorker = comm->getLastSource();
				job.resize(0);
				for(uint i = 0; i < jobSize && current < pool.size(); i++, current++)
					job.push_back(pool[current]);
				comm->sendIndividuals(job, iWorker);
			}
			// otherwise, do something useful
			else {
				evaluate(pool[current]);
				current++;
				remaining--;
			}
		}

		// while all individuals not received
		job.resize(0);
		uint remainingWorkers = comm->getCommSize() - 1;
		while(remaining > 0 || remainingWorkers > 0) {
			uint received = comm->recvDemeFitness(*deme, Comm::ANY_PROCESS);
			state->increaseEvaluations(received);
			remaining -= received;

			uint iWorker = comm->getLastSource();
			comm->sendIndividuals(job, iWorker);
			remainingWorkers--;
		}

		// elitism
		IndividualP random = selRandomOp->select(*deme);
		if(best->fitness->isBetterThan(random->fitness))
			replaceWith(random, best);

		for(uint i = 0; i < deme->size(); i++){
			ECF_LOG(state, 5, "deme[" + uint2str(i) + "]: " + dbl2str(deme->at(i)->fitness->getValue()));
		}
	}

	// WORKER
	else {
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
