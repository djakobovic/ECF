#ifndef Logger_h
#define Logger_h

#include <string>
#include <vector>
#include <fstream>

class State;
typedef boost::shared_ptr<State> StateP;


/**
 * \brief Log message if given log level is less or equal than current level.
 */
#define ECF_LOG(state, level, msg) \
	( ((state)->getLogger()->getLogLevel() >= (level)) ? (state)->getLogger()->log((level), (msg)) : (void) 0 )


/**
 * \brief Log error message (log always).
 */
#define ECF_LOG_ERROR(state, msg) \
	( (state)->getLogger()->log(1, (msg)) )


/**
 * \ingroup evol main
 * \brief Logging class - handles screen output and file logging.
 *
 * Current log levels (1-5): 1=only basic, essential information, 3=medium logging level (default), 5=logs everything
 */
class Logger
{
public:
	typedef struct {
		int logLevel;
		std::string message;
	} Log;

	Logger();
	~Logger();

	/**
	 * Puts a string message into existing log and marks it with appropriate logLevel.
	 */
	void log(int logLevel, std::string message);

	/**
	 * Saves current session log into a file named fileName. 
	 * If file with the same name already exists, log is appended to the end of the file.
	 */
	void saveTo(std::string fileName);

	/**
	 * Saves current session log into a configured logfile. 
	 * New log is appended to the end of the file.
	 */
	void saveTo(bool check = false);

	void saveToX(std::string fileName);

	void saveToX();

	void flushLog();	//!< deletes current logs

	/**
	 * Sets the log output frequency (in generations).
	 * If not set, the default is 1 (output each generation).
	 */
	void setLogFrequency(uint freq);

	void registerParameters(StateP state);

	/**
	 * Initialize Logger, read current logLevel from the Registry (configuration file)
	 */
	bool initialize(StateP state);

	bool operate(StateP);

	void closeLog()
	{	logFile_.close();	}

	uint getLogLevel()
	{	return currentLevel_;	}

protected:
	std::vector<Log> logs_;
	uint currentLevel_;
	uint logFrequency_;
	std::string logFileName_;
	bool bFileDefined_;
	std::ofstream logFile_;
	StateP state_;
};
typedef boost::shared_ptr<Logger> LoggerP;

#endif
