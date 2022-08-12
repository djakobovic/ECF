#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

/* Created: 2.3.2010.
*  Author: dkomlen
*/

/**
* \ingroup xcs
* \brief Environment for the XCS algorithm
*/
class Environment : public EvaluateOp {

protected:
	StateP state;

public:
	virtual GenotypeP getInput() = 0;
	//IDEJA: napraviti da se iz datoteke s parametrima uèita tag <problem>
	//i cijeli proslijedi oklini prilikom inicijalizacije

	//Method for Environment initialization
	//It is called at algorithm initialization (XCS::initialize)
	virtual bool initialize() =0;

	//Sets environment for next trial
	//It is called after isOver() returns true;
	virtual bool nextTrial() = 0;

	//Resets environment to initial state
	virtual bool reset() = 0;

	virtual bool isOver() { return true; };

	//Method used for action evaluation
	//Individual has two genotypes:
	//    - last input from the environment
	//    - action genotype
	virtual FitnessP evaluate (IndividualP ind) =0;

	//Used to determine if the experiment is exploit
	//Returns whether next input should be processed
	//usefull for testing the system
	virtual bool isExploit() { return false; };


	bool checkState(const StateP state) { return true; };

};
typedef boost::shared_ptr<Environment> EnvironmentP;

#endif