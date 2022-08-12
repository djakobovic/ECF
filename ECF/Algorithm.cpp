#include "ECF_base.h"

#ifdef _MPI

const int MASTER = 0;


void Algorithm::registerParallelParameters(StateP state)
{
	std::string *type = new std::string("eval");
	state->getRegistry()->registerEntry("parallel.type", (voidP) type, ECF::STRING,
		"implicit parallelization method: eval - evaluation, mut - mutation + eval");

	uint* jobSize = new uint(10);
	state->getRegistry()->registerEntry("parallel.jobsize", (voidP) jobSize, ECF::UINT,
		"implicit parallelization jobsize (individuals per job)");

	uint *sync = new uint(0);
	state->getRegistry()->registerEntry("parallel.sync", (voidP) sync, ECF::UINT,
		"implicit parallelization synchronicity: 0 - async, 1 - sync ");
}


bool Algorithm::initializeParallel(StateP state)
{
	state_ = state;
	comm_ = state->getCommunicator();
	selBestOp = static_cast<SelBestOpP> (new SelBestOp);
	totalEvaluations_ = wastedEvaluations_ = 0;
	bImplicitEvaluation_ = bImplicitMutation_ = false;

	// initialize protected members for population consistency
	uint demeSize = state->getPopulation()->getLocalDeme()->getSize();
	storedInds_.resize(demeSize);
	sentInds_.resize(demeSize);
	isConsistent_.resize(demeSize);

	if(!state_->getRegistry()->isModified("parallel.type"))
		bImplicitParallel_ = false;
	else
		bImplicitParallel_ = true;

	if(this->isParallel() && bImplicitParallel_) {
		ECF_LOG_ERROR(state_, "Error: implicit parallelization possible only with sequential algorithm!");
		throw "";
	}

	if((this->isParallel() || bImplicitParallel_)) {
		if(state->getPopulation()->getNoDemes() > state->getCommunicator()->getCommGlobalSize()) { 
			ECF_LOG_ERROR(state, "Error: number of processes must be equal or greater than the number of demes in parallel EA!");
			throw "";
		}
	}
	else if(state->getPopulation()->getNoDemes() != state->getCommunicator()->getCommGlobalSize()) {
		ECF_LOG_ERROR(state, "Error: number of processes must equal the number of demes in EA with sequential algorithm!");
		throw "";
	}

	if(!bImplicitParallel_)
		return true;

	voidP sptr = state->getRegistry()->getEntry("parallel.type");
	std::string parallelType = *((std::string*)sptr.get());

	if(parallelType == "eval") {
		bImplicitEvaluation_ = true;
	}
	else if(parallelType == "mut") {
		bImplicitMutation_ = true;
	}
	else {
		ECF_LOG_ERROR(state, "Error: unkown implicit parallelization mode!");
		throw "";
	}

	sptr = state->getRegistry()->getEntry("parallel.sync");
	bSynchronized_ = ((*((uint*) sptr.get())) != 0);

	voidP jobSizeP = state->getRegistry()->getEntry("parallel.jobsize");
	jobSize_ = *((uint*) jobSizeP.get());

	if(jobSize_ > state->getPopulation()->at(0)->getSize()) {
		ECF_LOG_ERROR(state, "Error: parallel.jobsize must be less or equal to the number of individuals!");
		throw "";
	}

	return true;
}


// create deme-sized vector of initialized individuals for receiving
void Algorithm::initializeImplicit(StateP state)
{
	for(uint i = 0; i < state->getPopulation()->at(0)->size(); i++) {
		IndividualP ind = (IndividualP) new Individual(state);
		ind->fitness = (FitnessP) state->getFitnessObject()->copy();
		demeCopy_.push_back(ind);
		requestIds_.push_back(0);
		requestMutIds_.push_back(0);
	}
	stored_.resize(state->getPopulation()->at(0)->size());
	currentBest_ = selBestOp->select(*(state->getPopulation()->at(0)));
}


bool Algorithm::advanceGeneration(StateP state)
{
	// implicitly parallel: worker processes go to work
	if(state->getCommunicator()->getCommRank() != 0 && bImplicitParallel_)
		return implicitParallelOperate(state);
	// otherwise, run the algorithm
	else
		return advanceGeneration(state, state->getPopulation()->at(0));
}


void Algorithm::implicitEvaluate(IndividualP ind)
{
	totalEvaluations_++;
	if(requestIds_[ind->index] > 0) {	// if already on evaluation
		wastedEvaluations_++;
		return;
	}

	DemeP workingDeme = state_->getPopulation()->getLocalDeme();

	if(!isMember(ind, requests_)) {
		requests_.push_back(ind);
		requestIds_[ind->index] = 1;	// fitness wanted

		// synchronous implicit evaluation: individual to be evaluated is removed from the deme
		// upon return, the individual is put at the end of deme
		// individual's index is _NOT_ its place in deme vector anymore!
		if(bSynchronized_) {
			stored_[ind->index] = ind;
			removeFrom(ind, *workingDeme);
		}
	}

	if(requests_.size() < jobSize_)
		return;

	if(comm_->messageWaiting()) {
		ECF_LOG(state_, 4, "Receiving evaluated individuals");
		std::vector<uint> indices = comm_->recvFitnessVector(demeCopy_, Comm::ANY_PROCESS);
		for(uint i = 0; i < indices.size(); i++) {
			if(requestIds_[indices[i]] > 0) {	// only if needs evaluation
				if(bSynchronized_) {	// return individual to deme
					stored_[indices[i]]->fitness = demeCopy_[indices[i]]->fitness;
					workingDeme->push_back(stored_[indices[i]]);
				}
				else
					workingDeme->at(indices[i])->fitness = demeCopy_[indices[i]]->fitness;
				requestIds_[indices[i]] = 0;
			}
		}

		uint iWorker = comm_->getLastSource();
		comm_->sendIndividuals(requests_, iWorker);
		requests_.resize(0);
	}
	else {
		ECF_LOG(state_, 4, "Evaluating locally...");
		IndividualP ind = requests_.back();
		ind->fitness = evalOp_->evaluate(ind);
		requestIds_[ind->index] = 0;
		if(bSynchronized_) {	// return individual to deme
			workingDeme->push_back(ind);
		}
		requests_.pop_back();
	}

}


uint Algorithm::implicitMutate(IndividualP ind)
{
	DemeP workingDeme = state_->getPopulation()->getLocalDeme();

	if(!isMember(ind, requestsMut_)) {
		requestsMut_.push_back(ind);
		requestMutIds_[ind->index] = 1;	// mutation needed

		if(bSynchronized_) {	// remove from deme
			removeFrom(ind, *workingDeme);
		}
	}
	if(requestsMut_.size() < jobSize_)
		return 0;

	if(comm_->messageWaiting()) {
		ECF_LOG(state_, 4, "Receiving mutated individuals");
		uint received = comm_->recvReplaceIndividuals(receivedMut_, Comm::ANY_PROCESS);
		currentBest_ = selBestOp->select(*workingDeme);

		for(uint i = 0; i < received; i++) {
			// if synchronized, return individual to deme
			if(bSynchronized_) {
				IndividualP newInd = (IndividualP) receivedMut_[i]->copy();
				newInd->index = receivedMut_[i]->index;
				workingDeme->push_back(newInd);
			}
			// replace individual only if mutation is needed
			// also: only if not replacing the current best
			else {
				if(requestMutIds_[receivedMut_[i]->index] > 0
					&& (workingDeme->at(receivedMut_[i]->index) != currentBest_ 
					|| receivedMut_[i]->fitness->isBetterThan(currentBest_->fitness))) {
						IndividualP newInd = (IndividualP) receivedMut_[i]->copy();
						workingDeme->replace(receivedMut_[i]->index, newInd);
				}
			}
			requestMutIds_[receivedMut_[i]->index] = 0;
		}

		uint iWorker = comm_->getLastSource();
		comm_->sendIndividuals(requestsMut_, iWorker);

		requestsMut_.resize(0);
		return received;
	}
	else {
		ECF_LOG(state_, 4, "Mutating locally...");
		std::vector<IndividualP> pool;
		pool.push_back(requestsMut_.back());
		requestMutIds_[requestsMut_.back()->index] = 0;
		requestsMut_.pop_back();

		if(bSynchronized_) {	// return individual to deme
			workingDeme->push_back(pool[0]);
		}
		uint mutated = mutation_->mutation(pool);
		return mutated;
	}
}


// implicit parallelization: worker processes
bool Algorithm::implicitParallelOperate(StateP state)
{
	ECF_LOG(state, 4, "Worker process initiating.");

	if(bImplicitEvaluation_) {
		myJob_.resize(0);
		comm_->sendFitness(myJob_, MASTER);
		uint myJobSize = 1;

		// while individuals to evaluate
		while(myJobSize != 0) {

			myJobSize = comm_->recvReplaceIndividuals(myJob_, MASTER);
			for(uint i = 0; i < myJobSize; i++)
				myJob_[i]->fitness = evalOp_->evaluate(myJob_[i]);

			comm_->sendFitness(myJob_, MASTER, myJobSize);
		}
	}

	// implicit mutation + evaluation
	else if(bImplicitMutation_) {
		std::vector<IndividualP> mutationPool;
		myJob_.resize(0);
		comm_->sendIndividuals(myJob_, MASTER);
		uint myJobSize = 1;

		// while individuals to mutate
		while(myJobSize != 0) {

			myJobSize = comm_->recvReplaceIndividuals(myJob_, MASTER);

			mutationPool.resize(myJobSize);
			for(uint i = 0; i < myJobSize; i++) {
				mutationPool[i] = myJob_[i];
			}

			mutation_->mutation(mutationPool);

			for(uint i = 0; i < myJobSize; i++)
				if(!mutationPool[i]->fitness->isValid())
					mutationPool[i]->fitness = evalOp_->evaluate(mutationPool[i]);

			comm_->sendIndividuals(myJob_, MASTER, myJobSize);
		}
	}

	return true;
}


void Algorithm::bcastTermination(StateP state)
{
	if(bImplicitParallel_) {
		// master: if termination not true, do nothing; otherwise, send workers empty jobs
		if(state->getCommunicator()->getCommRank() == 0 && state->getTerminateCond()) {
			std::vector<IndividualP> empty;
			for(uint iWorker = 1; iWorker < state->getCommunicator()->getCommSize(); iWorker++) {
				state->getCommunicator()->sendIndividuals(empty, iWorker);
			}
		}
		// worker: empty job received, set terminate for local process
		else if(state->getCommunicator()->getCommRank() != 0)
			state->setTerminateCond();

		return;
	}

	// explicitly parallel algorithm (synchronous)
	if(state->getCommunicator()->getCommRank() == 0) {
		for(uint i = 1; i < comm_->getCommSize(); i++)
			comm_->sendControlMessage(i, state->getTerminateCond());
	}
	else {
		if(comm_->recvControlMessage(0))
			state->setTerminateCond();
	}
}


void Algorithm::replaceWith(IndividualP oldInd, IndividualP newInd)
{
	if(!bSynchronized_) {
		state_->getPopulation()->at(0)->replace(oldInd->index, newInd);
		return;
	}

	DemeP workingDeme = state_->getPopulation()->at(0);

	for(uint i = 0; i < workingDeme->size(); i++) {
		if(workingDeme->at(i)->index == oldInd->index) {
			workingDeme->at(i) = newInd;
			newInd->index = oldInd->index;
			return;
		}
	}
}


bool Algorithm::initializePopulation(StateP state)
{
	CommunicatorP comm = state->getCommunicator();
	DemeP myDeme = state->getPopulation()->at(0);
	uint demeSize = (uint) myDeme->size();

	isConsistent_.assign(isConsistent_.size(), true);

	if(comm->getCommSize() == 1) {
		for(uint iInd = 0; iInd < demeSize; iInd++) {
			IndividualP ind = myDeme->at(iInd);
			state_->getContext()->evaluatedIndividual = ind;
			ECF_LOG(state_, 5, "Evaluating individual: " + ind->toString());
			ind->fitness = evalOp_->evaluate(ind);
			ECF_LOG(state_, 5, "New fitness: " + ind->fitness->toString());
			state_->increaseEvaluations();
		}
		return true;
	}

	if(comm->getCommRank() == 0) {
		uint jobsPerProcess = 4;
		uint jobSize = 1 + demeSize / comm->getCommSize() / jobsPerProcess;
		uint remaining = demeSize;
		std::vector<IndividualP> job;

		for(uint iInd = 0; iInd < demeSize; ) {
			for(uint i = 0; i < jobSize && iInd < demeSize; i++, iInd++) {
				myDeme->at(iInd)->fitness = (FitnessP) state->getFitnessObject()->copy();
				job.push_back(myDeme->at(iInd));
			}
			remaining -= comm->recvDemeFitness(*myDeme, MPI::ANY_SOURCE);
			uint iWorker = comm->getLastSource();
			comm->sendIndividuals(job, iWorker);
			job.resize(0);
		}

		int remainingWorkers = comm->getCommSize() - 1;
		while(remaining > 0 || remainingWorkers > 0) {
			remaining -= comm->recvDemeFitness(*myDeme, MPI::ANY_SOURCE);

			uint iWorker = comm->getLastSource();
			comm->sendIndividuals(job, iWorker);
			remainingWorkers--;
		}

		state->increaseEvaluations(demeSize);
	}

	else {
		std::vector<IndividualP> myJob_;
		myJob_.resize(0);
		comm->sendFitness(myJob_, MASTER);

		uint myJobSize;
		myJobSize = comm->recvReplaceIndividuals(myJob_, MASTER);

		// while individuals to evaluate
		while(myJobSize > 0) {
			for(uint i = 0; i < myJobSize; i++) {
				state_->getContext()->evaluatedIndividual = myJob_[i];
				ECF_LOG(state_, 5, "Evaluating individual: " + myJob_[i]->toString());
				myJob_[i]->fitness = evalOp_->evaluate(myJob_[i]);
				ECF_LOG(state_, 5, "New fitness: " + myJob_[i]->fitness->toString());
			}

			comm->sendFitness(myJob_, MASTER, myJobSize);
			myJobSize = comm->recvReplaceIndividuals(myJob_, MASTER);
		}
	}

	return true;
}


// called before genotype change
void Algorithm::storeIndividual(IndividualP ind)
{
	if(isConsistent_[ind->index]) {
		storedInds_[ind->index] = (IndividualP) ind->copy();
	}
	isConsistent_[ind->index] = false;
}


// called after individual evaluation
void Algorithm::setConsistency(IndividualP ind)
{
	isConsistent_[ind->index] = true;
	storedInds_[ind->index]->fitness = (FitnessP) ind->fitness->copy();
}


// called when sending individuals for evaluation
void Algorithm::storeGenotypes(std::vector<IndividualP>& pool)
{
	for(uint ind = 0; ind < pool.size(); ind++) {
		uint index = pool[ind]->index;

		// find first available slot for individual
		uint cid = 0;
		while(cid < sentInds_[index].size()) {
			if(!sentInds_[index][cid])
				break;
			cid++;
		}
		if(cid == sentInds_[index].size())
			sentInds_[index].resize(cid + 1);

		// assign cid, copy individual
		pool[ind]->cid = cid;
		sentInds_[index][cid] = (IndividualP) pool[ind]->copy();
	}
}


// called when fitness is restored
void Algorithm::restoreIndividuals(std::vector<uint> received)
{
	DemeP myDeme = state_->getPopulation()->getLocalDeme();

	for(uint ind = 0; ind < received.size(); ind++) {
		uint index = received[ind];
		uint cid = myDeme->at(index)->fitness->cid;

		// restore genotype
		if(isConsistent_[index] == false) {
			sentInds_[index][cid]->fitness = myDeme->at(index)->fitness;
			replaceWith(myDeme->at(index), sentInds_[index][cid]);

			isConsistent_[index] = true;
			storedInds_[index]->fitness = (FitnessP) myDeme->at(index)->fitness->copy();
		}
		// or reject new fitness
		else {
			myDeme->at(index)->fitness = (FitnessP) storedInds_[index]->fitness->copy();
		}
		sentInds_[index][cid].reset();
	}
}


// called at evolution end for fitness consistency
void Algorithm::restorePopulation()
{
	DemeP myDeme = state_->getPopulation()->getLocalDeme();
	uint demeSize = myDeme->getSize();

	for(uint ind = 0; ind < demeSize; ind++) {
		if(!isConsistent_[ind]) {
			(*myDeme)[ind] = (IndividualP) storedInds_[ind]->copy();
		}
	}
}


#else	// non _MPI


bool Algorithm::initializePopulation(StateP state)
{
	for(uint iDeme = 0; iDeme < state->getPopulation()->size(); iDeme++)
		for(uint iInd = 0; iInd < state->getPopulation()->at(iDeme)->size(); iInd++)
			evaluate(state->getPopulation()->at(iDeme)->at(iInd));
	return true;
}

#endif	// _MPI