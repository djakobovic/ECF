#include <iostream>
#include "ECF_base.h"


Logger::Logger()
{
	currentLevel_ = 3;
	bFileDefined_ = false;
	logFrequency_ = 1;
}


Logger::~Logger()
{
	logFile_.close();
}


void Logger::registerParameters(StateP state)
{
	uint *level = new uint(3);
	state->getRegistry()->registerEntry("log.level", (voidP) level, ECF::UINT,
		"log level; valid values are 1 (minimal) to 5 (verbose) (default: 3)");
	std::string *file = new std::string("");
	state->getRegistry()->registerEntry("log.filename", (voidP) file, ECF::STRING,
		"log filename (default: none)");
	uint *freq = new uint(1);
	state->getRegistry()->registerEntry("log.frequency", (voidP) freq, ECF::UINT,
		"log only every 'frequency' generations (default: 1)");
}


bool Logger::initialize(StateP state)
{
	if(logFile_.is_open())
		logFile_.close();

	//reads parameters from registry
	voidP sptr = state->getRegistry()->getEntry("log.level");
	currentLevel_ = *((uint*) sptr.get());
	sptr = state->getRegistry()->getEntry("log.frequency");
	logFrequency_ = *((uint*) sptr.get());

	if(state->getRegistry()->isModified("log.filename")) {
		sptr = state->getRegistry()->getEntry("log.filename");
		logFileName_ = *((std::string*) sptr.get());

		logFile_.open(logFileName_.c_str());
		if(logFile_)
			bFileDefined_ = true;
		else {
			throw std::string("Error: can't open logfile (") + logFileName_ + ")";
		}
	}
	else
		bFileDefined_ = false;

	state_ = state;
	return true;
}


void Logger::log(int logLevel, std::string message)
{
	if(currentLevel_ >= (uint) logLevel) {
		Log tmp;
		if(logLevel > 5) logLevel = 5;
		if(logLevel < 0) logLevel = 0;

#ifdef _MPI
		message = uint2str(state_->getCommunicator()->getCommGlobalRank()) + ": " + message;
#endif

		logs_.push_back(tmp);
		logs_.back().logLevel = logLevel;
		logs_.back().message = message;

		if(logLevel == 1 || state_->getGenerationNo() % logFrequency_ == 0)
			std::cout << logs_.back().message << std::endl;
	}
}


void Logger::saveTo(std::string fileName) 
{
	std::ofstream logFile(fileName.c_str(),std::ios::app);
	for (uint i = 0; i < logs_.size(); ++i){
		logFile << logs_[i].message << std::endl;
	}

	logFile.close(); 
	logs_.clear();
}


void Logger::saveTo(bool check) 
{
	if(!bFileDefined_) {
		logs_.clear();
		return;
	}

	if(!check && state_->getGenerationNo() > 1 && state_->getGenerationNo() % logFrequency_ != 0) {
		logs_.clear();
		return;
	}

	std::stringstream logOutput;

	for (uint i = 0; i < logs_.size(); ++i) {
		logOutput << logs_[i].message << std::endl;
	}

#ifdef _MPI
	if(state_->getCommunicator()->getCommGlobalSize() != 1) {

		if(state_->getCommunicator()->getCommGlobalRank() == 0) {
			std::string remoteLogs = state_->getCommunicator()->recvLogsGlobal();
			logFile_ << logOutput.str() << remoteLogs;
		}
		else {
			state_->getCommunicator()->sendLogsGlobal(logOutput.str());
		}
	}

	else
		logFile_ << logOutput.str();
#else
	logFile_ << logOutput.str();
#endif

	logs_.clear();
}


void Logger::saveToX(std::string fileName)
{
	std::ofstream logFile(fileName.c_str(), std::ios::app);
	logFile<<"<log>"<<std::endl;

	for (uint i = 0; i < logs_.size(); ++i){	
		logFile <<"<message logLevel=\""<< logs_[i].logLevel << "\">" << logs_[i].message <<"</message>"<< std::endl;
	}
	logFile<<"</log>";

	logFile.close();
	logs_.clear();
}


void Logger::saveToX()
{
	if(!bFileDefined_)
		return;
	logFile_ << "<log>" << std::endl;
	for (uint i = 0; i < logs_.size(); ++i){	
		logFile_ <<"<message logLevel=\""<< logs_[i].logLevel << "\">" << logs_[i].message <<"</message>"<< std::endl;
	}
	logFile_ << "</log>";

	logs_.clear();
}


void Logger::flushLog() 
{
	logs_.clear();
}


bool Logger::operate(StateP state)
{
	saveTo();
	return true;
}


void Logger::setLogFrequency(uint freq)
{
	logFrequency_ = freq;
}
