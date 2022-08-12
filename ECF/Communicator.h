#ifndef Communicator_h
#define Communicator_h

class Individual;
typedef boost::shared_ptr<Individual> IndividualP;
class State;
typedef boost::shared_ptr<State> StateP;


namespace Comm
{

// message tags:
const int T_DEFAULT = 0;
const int T_CONTROL = 99;
const int T_CONTINUE = 1;
const int T_TERMINATE = 2;
const int T_VALUES = 3;
const int T_LOGS = 4;
const int T_FINAL = 5;
const int T_DATA = 6;

// controls:
const int CONTINUE = 1;
const int TERMINATE = 0;

// timing categories
enum timing
{	COMP, IDLE, SEND, RECV, PACK, UNPACK	};

#ifdef _MPI

// MPI defines
const int ANY_PROCESS = MPI::ANY_SOURCE;

/**
 * \brief Communicator class for interprocess communication.
 *
 * Wraps MPI calls for message passing between processes.
 * Note: will be reimplemented as an abstract class with MPI specialization.
 */
class Communicator
{
protected:
	// MPI parameters
	uint mpiGlobalSize_, mpiGlobalRank_;
	uint mpiSize_, mpiRank_;
	std::string processorName_;
	StateP state_;
	MPI::Status status_;
	MPI::Status controlStatus_;
	int logLevel_;
	std::vector<uint> demeMasters;
	bool bInitialized_;

	// communicators
	/**
	framework communicator - equivalent to MPI_COMM_WORLD
	Used by:
	- State
		- for termination condition messages
	- Migration
		- for individual exchange between demes
	- Population
		- for statistics and hall-of-fame updates
	- Logger
		- for accumulation of logs to process 0
	*/
	MPI::Intercomm frameworkComm_;

	/**
	deme communicator - initially includes all processes (MPI_COMM_WORLD) but may be split into subcomms if multiple deme population is used
	Used by:
	- the active algorithm, for algorithm implementation and broadcast of termination signal
	*/
	MPI::Intercomm demeComm_;

	// timing
	double currentTime_, lastTime_;
	double beginTime_, endTime_;
	double idleTime_;
	double sendTime_;
	double recvTime_;
	double compTime_;
	double packTime_, unpackTime_;
	uint sendCnt_;
	uint recvCnt_;
	double time(enum timing T);
public:
	Communicator();
	bool initialize(StateP, int, char**);
	bool finalize();
	bool isInitialized()
	{	return bInitialized_;	}

	// send methods:
	bool sendIndividuals(std::vector<IndividualP>, uint, uint nIndividuals = 0);
	bool sendIndividualsGlobal(std::vector<IndividualP>, uint, uint nIndividuals = 0);
	bool sendFitness(std::vector<IndividualP>, uint, uint nIndividuals = 0);
	bool sendValuesGlobal(std::vector<double>, uint);
	bool sendLogsGlobal(std::string, uint iProcess = 0, bool blocking = false);
	bool sendDataGlobal(voidP, uint, uint);

	// receive methods:
	uint recvDemeIndividuals(std::vector<IndividualP>&, uint);
	std::vector<IndividualP> recvIndividuals(uint iProcess = MPI::ANY_SOURCE);
	std::vector<IndividualP> recvIndividualsGlobal(uint iProcess = MPI::ANY_SOURCE);
	uint recvReplaceIndividuals(std::vector<IndividualP>&, uint);
	uint recvDemeFitness(std::vector<IndividualP>&, uint);
	std::vector<uint> recvFitnessVector(std::vector<IndividualP>&, uint);
	std::vector<double> recvValuesGlobal(uint iProcess = MPI::ANY_SOURCE);
	std::string recvLogsGlobal();
	voidP recvDataGlobal(uint iProcess = MPI::ANY_SOURCE);

	// other:
	bool messageWaiting(uint iProcess = MPI::ANY_SOURCE, uint tag = MPI::ANY_TAG);
	void synchronize();
	bool sendControlMessage(uint, int);
	int recvControlMessage(uint);
	bool sendTerminateMessage(uint, bool);
	bool recvTerminateMessage(uint);
	bool checkTerminationMessage(uint master = 0);
	uint createDemeCommunicator(uint nDemes);
	uint getDemeMaster(uint iDeme);

	// info:
	uint getLastSource();
	uint getCommRank()
	{	return mpiRank_;	}
	uint getCommSize()
	{	return mpiSize_;	}
	uint getCommGlobalRank()
	{	return mpiGlobalRank_;	}
	uint getCommGlobalSize()
	{	return mpiGlobalSize_;	}

};
typedef boost::shared_ptr<Communicator> CommunicatorP;

#else // non _MPI

class Communicator
{
public:
	bool initialize(StateP state, int argc, char** argv)
	{	return true;	}
	bool finalize()
	{	return true;	}
	uint getCommRank()
	{	return 0;	}
	uint getCommGlobalRank()
	{	return 0;	}
	uint getCommSize()
	{	return 0;	}

};
typedef boost::shared_ptr<Communicator> CommunicatorP;
#endif // _MPI

}	// namespace

typedef boost::shared_ptr<Comm::Communicator> CommunicatorP;


#endif // Communicator_h