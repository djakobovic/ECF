#include "ECF_base.h"
#include <string.h>

namespace Comm
{

#ifdef _MPI

Communicator::Communicator()
{
	bInitialized_ = false;
	idleTime_ = sendTime_ = recvTime_ = compTime_ = packTime_ = unpackTime_ = 0;
	logLevel_ = 4;
	sendCnt_ = recvCnt_ = 0;
}


bool Communicator::initialize(StateP state, int argc, char** argv)
{
	idleTime_ = sendTime_ = recvTime_ = compTime_ = packTime_ = unpackTime_ = 0;

	// multiple runs
	if(bInitialized_) {
		beginTime_ = lastTime_ = MPI::Wtime();
		ECF_LOG(state_, 2, "Process " + uint2str(mpiGlobalRank_) + " of "
			+ uint2str(mpiGlobalSize_) + " on " + processorName_);
		return true;
	}

	state_ = state;

	MPI::Init(argc, argv);

	mpiGlobalSize_ = MPI::COMM_WORLD.Get_size();
	mpiGlobalRank_ = MPI::COMM_WORLD.Get_rank();

	demeComm_ = MPI::COMM_WORLD.Dup();
	mpiSize_ = demeComm_.Get_size();
	mpiRank_ = demeComm_.Get_rank();

	frameworkComm_ = MPI::COMM_WORLD.Dup();

	char *pp, processor_name[MPI_MAX_PROCESSOR_NAME];
	pp = &processor_name[0];
	int namelen;
	MPI_Get_processor_name(processor_name, &namelen);
	processorName_ = processor_name;

	beginTime_ = lastTime_ = MPI::Wtime();

	ECF_LOG(state_, 2, "Process " + uint2str(mpiGlobalRank_) + " of "
		+ uint2str(mpiGlobalSize_) + " on " + processorName_);

	bInitialized_ = true;

	return true;
}


// zove se na pocetku prije evolucije
// stvara se posebni komunikator za svaki deme (lokalni kontekst za algoritam)
uint Communicator::createDemeCommunicator(uint nDemes)
{
	// TODO: parametrize deme distribution among processes
	uint myColor = mpiGlobalRank_ % nDemes;
	demeMasters.resize(nDemes);
	for(uint i = 0; i < nDemes; i++)
		demeMasters[i] = i;

	demeComm_ = MPI::COMM_WORLD.Split(myColor, mpiGlobalRank_);
	mpiSize_ = demeComm_.Get_size();
	mpiRank_ = demeComm_.Get_rank();

	std::stringstream log;
	log << "Global process " << mpiGlobalRank_ << " joined deme communicator with index ";
	log << myColor << " (local rank: " << mpiRank_ << " of " << mpiSize_ << ")";
	ECF_LOG(state_, 2, log.str());

	return myColor;	// deme index
}


bool Communicator::finalize()
{
	if(!bInitialized_)
		return true;

	endTime_ = MPI::Wtime();

	std::stringstream times;
	times << "Process " << mpiGlobalRank_ << ": total MPI time: " << endTime_ - beginTime_;
	times << ", COMP: " << compTime_;
	times << ", IDLE: " << idleTime_;
	times << ", SEND: " << sendTime_;
	times << ", RECV: " << recvTime_;
	times << ", PACK: " << packTime_;
	times << ", UNPACK: " << unpackTime_ << std::endl;

	// collect and log process times at process 0
	if(mpiGlobalRank_ == 0) {
		std::string message;
		MPI::Status status;

		for(uint iProcess = 1; iProcess < mpiGlobalSize_; iProcess++) {
			frameworkComm_.Probe(iProcess, T_FINAL, status);
			uint size = status.Get_count(MPI::CHAR);
			message.resize(size);
			frameworkComm_.Recv(&message[0], size, MPI::CHAR, iProcess, T_FINAL, status);

			times << message;
		}
		ECF_LOG(state_, 2, times.str());
		state_->getLogger()->saveTo(true);
	}
	else {
		std::string message = times.str();
		frameworkComm_.Send(&message[0], (int) message.size(), MPI::CHAR, 0, T_FINAL);
	}

	if(!state_->getBatchMode())
		MPI::Finalize();

	return true;
}

/**
 * Gets current time and adds elapsed time to a category
 * \return: elapsed time (since last function call)
 */
double Communicator::time(enum timing T)
{
	currentTime_ = MPI::Wtime();
	double elapsed = currentTime_ - lastTime_;
	lastTime_ = currentTime_;

	switch(T) {
	case IDLE:
		idleTime_ += elapsed;
		break;
	case COMP:
		compTime_ += elapsed;
		break;
	case SEND:
		sendTime_ += elapsed;
		break;
	case RECV:
		recvTime_ += elapsed;
		break;
	case PACK:
		packTime_ += elapsed;
		break;
	case UNPACK:
		unpackTime_ += elapsed;
		break;
	}

	return 1000 * elapsed;
}


uint Communicator::getDemeMaster(uint iDeme)
{
	return demeMasters[iDeme];
}


uint Communicator::getLastSource()
{
	return status_.Get_source();
}


void Communicator::synchronize()
{
	MPI::COMM_WORLD.Barrier();
}


// provjerava (trenutno) ima li neka pristigla poruka
bool Communicator::messageWaiting(uint iProcess, uint tag)
{
	return demeComm_.Iprobe(iProcess, tag, status_);
}


bool Communicator::sendControlMessage(uint iProcess, int control)
{
	demeComm_.Send(&control, sizeof(int), MPI::BYTE, iProcess, T_CONTROL);
	return true;
}


int Communicator::recvControlMessage(uint iProcess)
{
	int control;
	demeComm_.Recv(&control, sizeof(int), MPI::BYTE, iProcess, T_CONTROL);
	return control;
}


bool Communicator::sendTerminateMessage(uint iProcess, bool termination)
{
	ECF_LOG(state_, logLevel_, "Sending terminate message to process " + uint2str(iProcess));
	uint tag = (termination == true) ? T_TERMINATE : T_CONTINUE;
	frameworkComm_.Send(&termination, sizeof(bool), MPI::BYTE, iProcess, tag);
	return true;
}


bool Communicator::recvTerminateMessage(uint iProcess)
{
	bool termination;
	frameworkComm_.Recv(&termination, sizeof(bool), MPI::BYTE, iProcess, MPI::ANY_TAG, controlStatus_);
	if(controlStatus_.Get_tag() == T_TERMINATE)
		termination = true;
	if(controlStatus_.Get_tag() == T_CONTINUE)
		termination = false;
	return termination;
}


bool Communicator::checkTerminationMessage(uint master)
{
	if(frameworkComm_.Iprobe(master, MPI::ANY_TAG, controlStatus_)) {
		if(controlStatus_.Get_tag() == T_TERMINATE) {
			return true;
		}
		else
			recvTerminateMessage(master);
	}
	return false;
}


// salje prvih nIndividuals jedinki iz zadanog vektora (sve ako je nInvididuals == 0)
// svakoj jedinki se dodaje njen indeks u _deme_ (ne iz vektora pool!)
bool Communicator::sendIndividuals(std::vector<IndividualP> pool, uint iProcess, uint nIndividuals)
{
	time(COMP);

	XMLNode xAll, xIndividual;
	xAll = XMLNode::createXMLTopNode("Pack");

	if(nIndividuals == 0)
		nIndividuals = (uint) pool.size();
	xAll.addAttribute("size", uint2str(nIndividuals).c_str());

	for(uint ind = 0; ind < nIndividuals; ind++) {
		pool[ind]->write(xIndividual);
		xIndividual.addAttribute("i", uint2str(pool[ind]->index).c_str());
		xIndividual.addAttribute("c", uint2str(pool[ind]->cid).c_str());
		xAll.addChild(xIndividual);
	}
	char *message = xAll.createXMLString(0);
	//std::string message = m;

	double createTime = time(PACK);

	//demeComm_.Send(message.data(), (int) message.length(), MPI::CHAR, iProcess, 0);
	demeComm_.Send(message, (int) strlen(message) + 1, MPI::CHAR, iProcess, T_DEFAULT);

	double sendTime = time(SEND);

	std::stringstream log;
	log << "sent " << nIndividuals << " individuals, " << strlen(message) << " bytes (P: " << createTime << " | S: " << sendTime << ")";
	ECF_LOG(state_, logLevel_, log.str());

	freeXMLString(message);

	return true;
}


// isto kao i gornja fja, samo sa frameworkComm_
bool Communicator::sendIndividualsGlobal(std::vector<IndividualP> pool, uint iProcess, uint nIndividuals)
{
	time(COMP);

	XMLNode xAll, xIndividual;
	xAll = XMLNode::createXMLTopNode("Pack");

	if(nIndividuals == 0)
		nIndividuals = (uint) pool.size();
	xAll.addAttribute("size", uint2str(nIndividuals).c_str());

	for(uint ind = 0; ind < nIndividuals; ind++) {
		pool[ind]->write(xIndividual);
		xIndividual.addAttribute("i", uint2str(pool[ind]->index).c_str());
		xIndividual.addAttribute("c", uint2str(pool[ind]->cid).c_str());
		xAll.addChild(xIndividual);
	}
	char *message = xAll.createXMLString(0);

	double createTime = time(PACK);

	frameworkComm_.Send(message, (int) strlen(message) + 1, MPI::CHAR, iProcess, T_DEFAULT);

	double sendTime = time(SEND);

	std::stringstream log;
	log << "sent " << nIndividuals << " individuals (global), " << strlen(message) << " bytes (P: " << createTime << " | S: " << sendTime << ")";
	ECF_LOG(state_, logLevel_, log.str());

	freeXMLString(message);

	return true;
}


// prima jednike na mjesto postojecih u deme objektu
// cita se indeks iz poruke i jedinka se stavlja na odgovarajuce mjesto u deme vektoru
uint Communicator::recvDemeIndividuals(std::vector<IndividualP>& deme, uint iProcess)
{
	XMLNode xAll, xIndividual;
	MPI::Status status;
	std::string message;

	time(COMP);

	demeComm_.Probe(iProcess, MPI::ANY_TAG, status);

	double idle = time(IDLE);

	uint length = status.Get_count(MPI::CHAR);
	message.resize(length + 1);
	demeComm_.Recv(&message[0], length, MPI::CHAR, iProcess, T_DEFAULT, status);

	double recv = time(RECV);

	xAll = XMLNode::parseString(message.c_str(), "Pack");
	uint nIndividuals = atoi(xAll.getAttribute("size"));
	uint index, cid;
	for(uint i = 0; i < nIndividuals; i++) {
		//xIndividual = xAll.getChildNode("Individual", i);
		xIndividual = xAll.getChildNode(i);	// ovo bi trebalo biti brze...?
		index = atoi(xIndividual.getAttributeValue(1));
		cid = atoi(xIndividual.getAttributeValue(2));
		deme[index]->read(xIndividual);
		deme[index]->cid = cid;
	}
	status_ = status;

	double read = time(UNPACK);

	std::stringstream log;
	log << "received " << nIndividuals << " individuals, " << length << " bytes (";
	log << "I: " << idle << " | R: " << recv << " | U: " << read << ")";
	ECF_LOG(state_, logLevel_, log.str());

	return nIndividuals;
}


// prima jedinke u novi vektor
// ujedno stvara i inicijalizira pristigle jedinke i stvara njihov fitness
std::vector<IndividualP> Communicator::recvIndividuals(uint iProcess)
{
	XMLNode xAll, xIndividual;
	MPI::Status status;
	std::string message;
	std::vector<IndividualP> pack;

	time(COMP);

	demeComm_.Probe(iProcess, MPI::ANY_TAG, status);

	double idle = time(IDLE);

	uint length = status.Get_count(MPI::CHAR);
	message.resize(length + 1);
	demeComm_.Recv(&message[0], length, MPI::CHAR, iProcess, T_DEFAULT, status);

	double recv = time(RECV);

	xAll = XMLNode::parseString(message.c_str(), "Pack");
	uint nIndividuals = atoi(xAll.getAttribute("size"));
	uint index, cid;
	for(uint i = 0; i < nIndividuals; i++) {
		//xIndividual = xAll.getChildNode("Individual", i);
		xIndividual = xAll.getChildNode(i);	// ovo bi trebalo biti brze...?
		pack.push_back((IndividualP) new Individual(state_));
		index = atoi(xIndividual.getAttributeValue(1));
		cid = atoi(xIndividual.getAttributeValue(2));

		pack[i]->index = index;
		pack[i]->cid = cid;
		pack[i]->fitness = (FitnessP) state_->getFitnessObject()->copy();
		pack[i]->read(xIndividual);
	}
	status_ = status;

	double read = time(UNPACK);

	std::stringstream log;
	log << "received " << nIndividuals << " individuals, " << length << " bytes (";
	log << "I: " << idle << " | R: " << recv << " | U: " << read << ")";
	ECF_LOG(state_, logLevel_, log.str());

	return pack;
}


// isto kao gornja fja, samo uz frameworkComm_
std::vector<IndividualP> Communicator::recvIndividualsGlobal(uint iProcess)
{
	XMLNode xAll, xIndividual;
	MPI::Status status;
	std::string message;
	std::vector<IndividualP> pack;

	time(COMP);

	frameworkComm_.Probe(iProcess, T_DEFAULT, status);

	double idle = time(IDLE);

	uint length = status.Get_count(MPI::CHAR);
	message.resize(length);
	frameworkComm_.Recv(&message[0], length, MPI::CHAR, iProcess, T_DEFAULT, status);

	double recv = time(RECV);

	xAll = XMLNode::parseString(message.c_str(), "Pack");
	uint nIndividuals = atoi(xAll.getAttribute("size"));
	uint index, cid;
	for(uint i = 0; i < nIndividuals; i++) {
		xIndividual = xAll.getChildNode(i);
		pack.push_back((IndividualP) new Individual(state_));
		index = atoi(xIndividual.getAttributeValue(1));
		cid = atoi(xIndividual.getAttributeValue(2));

		pack[i]->index = index;
		pack[i]->cid = cid;
		pack[i]->fitness = (FitnessP) state_->getFitnessObject()->copy();
		pack[i]->read(xIndividual);
	}
	status_ = status;

	double read = time(UNPACK);

	std::stringstream log;
	log << "received " << nIndividuals << " individuals, " << length << " bytes (";
	log << "I: " << idle << " | R: " << recv << " | U: " << read << ")";
	ECF_LOG(state_, logLevel_, log.str());

	return pack;
}


// prima jedinke u postojeci vektor jedinki (bez obzira na indekse)
// velicina vektora se _povecava_ na broj primljenih jedinki
// ako je jedinki manje, vektor se _ne smanjuje_
// vraca broj primljenih jedinki
uint Communicator::recvReplaceIndividuals(std::vector<IndividualP>& pool, uint iProcess)
{
	XMLNode xAll, xIndividual;
	MPI::Status status;

	time(COMP);

	demeComm_.Probe(iProcess, MPI::ANY_TAG, status);

	double idle = time(IDLE);

	uint length = status.Get_count(MPI::CHAR);
	char *message = new char[length + 1];
	demeComm_.Recv(&message[0], length, MPI::CHAR, iProcess, T_DEFAULT, status);
	status_ = status;

	double recv = time(RECV);

	xAll = XMLNode::parseString(message, "Pack");
	uint nIndividuals = atoi(xAll.getAttribute("size"));
	uint poolSize = (uint) pool.size();

	if(poolSize < nIndividuals) {
		pool.resize(nIndividuals);
		for(uint i = poolSize; i < nIndividuals; i++) {
			pool[i] = (IndividualP) new Individual(state_);
			pool[i]->fitness = (FitnessP) state_->getFitnessObject()->copy();
		}
	}

	uint index, cid;
	for(uint i = 0; i < nIndividuals; i++) {
		xIndividual = xAll.getChildNode(i);	// ovo bi trebalo biti brze...?
		index = atoi(xIndividual.getAttributeValue(1));
		cid = atoi(xIndividual.getAttributeValue(2));
		pool[i]->index = index;
		pool[i]->cid = cid;
		pool[i]->read(xIndividual);
	}

	double read = time(UNPACK);

	std::stringstream log;
	log << "received " << nIndividuals << " individuals, " << length << " bytes (I: " << idle << " | R: " << recv << " | U: " << read << ")";
	ECF_LOG(state_, logLevel_, log.str());

	delete [] message;

	return nIndividuals;
}


// salje fitnese jedinki iz zadanog vektora
// fitnesi se pakiraju kao jedinke bez genotipa (atribut "i" je prvi po redu!)
// svakoj jedinki se dodaje njen indeks iz _deme_ (ne iz vektora pool!)
// ako je nIndividuals != 0, salje se samo toliko prvih iz vektora
bool Communicator::sendFitness(std::vector<IndividualP> pool, uint iProcess, uint nIndividuals)
{
	time(COMP);

	XMLNode xAll, xIndividual, xFitness;
	xAll = XMLNode::createXMLTopNode("Pack");

	if(nIndividuals == 0)
		nIndividuals = (uint) pool.size();
	xAll.addAttribute("size", uint2str(nIndividuals).c_str());

	for(uint ind = 0; ind < nIndividuals; ind++) {
		xIndividual = XMLNode::createXMLTopNode("Individual");
		xIndividual.addAttribute("i", uint2str(pool[ind]->index).c_str());
		xIndividual.addAttribute("c", uint2str(pool[ind]->cid).c_str());

		pool[ind]->fitness->write(xFitness);
		xIndividual.addChild(xFitness);
		xAll.addChild(xIndividual);
	}
	char *message = xAll.createXMLString(0);

	double pack = time(PACK);

	demeComm_.Send(message, (int) strlen(message) + 1, MPI::CHAR, iProcess, T_DEFAULT);

	double send = time(SEND);

	std::stringstream log;
	log << "sent " << nIndividuals << " fitness objects, " << strlen(message) << " bytes (P: " << pack << " | S: " << send << ")";
	ECF_LOG(state_, logLevel_, log.str());

	freeXMLString(message);

	return true;
}


// prima fitnese na mjesto postojecih u deme objektu
// fitnesi zu zapakirani u jedinke bez genotipa (atribut "i" je prvi po redu!)
// cita se indeks iz poruke i fitnes jedinke se stavlja na odgovarajuce mjesto u deme vektoru
uint Communicator::recvDemeFitness(std::vector<IndividualP>& deme, uint iProcess)
{
	XMLNode xAll, xIndividual, xFitness;
	MPI::Status status;
	std::string message;

	time(COMP);

	demeComm_.Probe(iProcess, MPI::ANY_TAG, status);

	double idle = time(IDLE);

	uint length = status.Get_count(MPI::CHAR);
	message.resize(length);
	demeComm_.Recv(&message[0], length, MPI::CHAR, iProcess, T_DEFAULT, status);

	double recv = time(RECV);

	xAll = XMLNode::parseString(message.c_str(), "Pack");
	uint nIndividuals = atoi(xAll.getAttribute("size"));
	uint index, cid;
	for(uint i = 0; i < nIndividuals; i++) {
		xIndividual = xAll.getChildNode(i);	// ovo bi trebalo biti brze...?
		index = atoi(xIndividual.getAttributeValue(0));
		cid = atoi(xIndividual.getAttributeValue(1));
		xFitness = xIndividual.getChildNode(0);
		deme[index]->fitness->read(xFitness);
		deme[index]->fitness->cid = cid;

	}
	status_ = status;

	double read = time(UNPACK);

	std::stringstream log;
	log << "received " << nIndividuals << " fitnes objects, " << length << " bytes (I: " << idle << " | R: " << recv << " | U: " << read << ")";
	ECF_LOG(state_, logLevel_, log.str());

	return nIndividuals;
}


// prima fitnese u vektor fitnesa
// fitnesi dolaze zapakirani kao jedinke bez genotipa (atribut "i" je prvi po redu!)
// fitnesi se stavljaju u vektor (prvi argument) na mjesto koje pise kao indeks jedinke u poruci
// funkcija vraca vektor indeksa jedinki primljenih fitnesa
std::vector<uint> Communicator::recvFitnessVector(std::vector<IndividualP>& deme, uint iProcess)
{
	XMLNode xAll, xIndividual, xFitness;
	MPI::Status status;
	std::string message;
	std::vector<uint> indices;

	time(COMP);

	demeComm_.Probe(iProcess, MPI::ANY_TAG, status);

	double idle = time(IDLE);

	uint length = status.Get_count(MPI::CHAR);
	message.resize(length);
	demeComm_.Recv(&message[0], length, MPI::CHAR, iProcess, T_DEFAULT, status);

	double recv = time(RECV);

	xAll = XMLNode::parseString(message.c_str(), "Pack");
	uint nIndividuals = atoi(xAll.getAttribute("size"));
	uint index, cid;
	for(uint i = 0; i < nIndividuals; i++) {
		xIndividual = xAll.getChildNode(i);
		index = atoi(xIndividual.getAttributeValue(0));
		cid = atoi(xIndividual.getAttributeValue(1));
		xFitness = xIndividual.getChildNode(0);

		deme[index]->fitness->read(xFitness);
		deme[index]->fitness->cid = cid;
		indices.push_back(index);
	}
	status_ = status;

	double read = time(UNPACK);

	std::stringstream log;
	log << "received " << nIndividuals << " fitnes objects, " << length << " bytes (I: " << idle << " | R: " << recv << " | U: " << read << ")";
	ECF_LOG(state_, logLevel_, log.str());

	return indices;
}


// salje vektor vrijednosti u globalnom komunikatoru
bool Communicator::sendValuesGlobal(std::vector<double> values, uint iProcess)
{
	time(COMP);

	frameworkComm_.Send(&values[0], (int) values.size(), MPI::DOUBLE, iProcess, T_VALUES);

	time(SEND);

	std::stringstream log;
	log << "sent " << values.size() << " doubles";
	ECF_LOG(state_, logLevel_, log.str());

	return true;
}


// prima vektor vrijednosti u globalnom komunikatoru
std::vector<double> Communicator::recvValuesGlobal(uint iProcess)
{
	std::vector<double> values;
	MPI::Status status;

	time(COMP);

	frameworkComm_.Probe(iProcess, T_VALUES, status);

	double idle = time(IDLE);

	uint size = status.Get_count(MPI::DOUBLE);
	values.resize(size);
	frameworkComm_.Recv(&values[0], size, MPI::DOUBLE, iProcess, T_VALUES, status);

	double recv = time(RECV);

	std::stringstream log;
	log << "received " << values.size() << " doubles";
	ECF_LOG(state_, logLevel_, log.str());

	return values;
}


// prima pristigle log poruke od bilo kojih procesa u globalnom komunikatoru
std::string Communicator::recvLogsGlobal()
{
	std::string logs = "", message;
	uint logCount = 0;
	MPI::Status status;

	time(COMP);

	while(frameworkComm_.Iprobe(MPI::ANY_SOURCE, T_LOGS, status)) {
		uint iProcess = status.Get_source();
		uint length = status.Get_count(MPI::CHAR);
		message.resize(length);
		frameworkComm_.Recv(&message[0], length, MPI::CHAR, iProcess, T_LOGS, status);
		logs += message;
		logCount++;
	}

	time(RECV);

	std::stringstream log;
	log << "received " << logCount << " logs";
	ECF_LOG(state_, logLevel_, log.str());

	return logs;
}


// salje log poruke u globalnom komunikatoru
bool Communicator::sendLogsGlobal(std::string logs, uint iProcess, bool blocking)
{
	time(COMP);

	MPI::Request request;

	if(blocking)
		frameworkComm_.Send(&logs[0], (int) logs.size(), MPI::CHAR, iProcess, T_LOGS);
	else
		request = frameworkComm_.Isend(&logs[0], (int) logs.size(), MPI::CHAR, iProcess, T_LOGS);

	time(SEND);

	std::stringstream log;
	log << "sent " << logs.size() << " log chars";
	ECF_LOG(state_, logLevel_, log.str());

	return true;
}


// slanje bilo kakvih podataka u globalnom komunikatoru
bool Communicator::sendDataGlobal(voidP data, uint size, uint iProcess)
{
	time(COMP);

	frameworkComm_.Send(data.get(), size, MPI::BYTE, iProcess, T_DATA);

	time(SEND);

	std::stringstream log;
	log << "sent " << size << " bytes";
	ECF_LOG(state_, logLevel_, log.str());

	return true;
}


// primanje bilo kakvih podataka u globalnom komunikatoru
voidP Communicator::recvDataGlobal(uint iProcess)
{
	MPI::Status status;
	voidP data;

	time(COMP);

	frameworkComm_.Probe(iProcess, T_DATA, status);

	time(IDLE);

	uint size = status.Get_count(MPI::BYTE);
	data = (voidP) new char[size / sizeof(char) + 1];
	frameworkComm_.Recv(data.get(), size, MPI::BYTE, iProcess, T_DATA, status);

	time(RECV);

	std::stringstream log;
	log << "received " << size << " bytes";
	ECF_LOG(state_, logLevel_, log.str());

	return data;
}

#endif

} // namespace
