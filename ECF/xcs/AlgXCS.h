#ifndef XCS_h
#define XCS_h

#include "../Algorithm.h"
#include "../bitstring/BitString.h"

#include <set>

#include "XCSParams.h"
#include "Classifier.h"
#include "ClassifierParams.h"
#include "Environment.h"
#include <fstream>
#include <string>

//#define XCS_STEP_DEBUG
//#define XCS_DEBUG


/** \defgroup xcs XCS learning classifier system
	\ingroup algorithms
*/


/**
* \brief XCS classifier system
* \ingroup algorithms xcs
*/
class XCS : public Algorithm
{
public:
	XCS();
	bool advanceGeneration(StateP state, DemeP deme);
	bool initialize(StateP state);
	void registerParameters(StateP state);
	
protected:
	SelectionOperatorP selRandomOp, selWorstOp, selFitPropOp;
	
private:
	void test (DemeP deme); //MAKNI 

	unsigned long long int time;
	std::vector<ClassifierP> populationSet; //[P]
	EnvironmentP environment;
	XCSParamsP params;

	//XCS methods:
	std::vector<ClassifierP> generateMatchSet(StateP state, DemeP deme, GenotypeP input);
	ClassifierP cover (StateP state, DemeP deme, GenotypeP input, std::vector<ClassifierP> matchSet);
	std::map<int, double> generatePA(std::vector<ClassifierP> matchSet);
	int selectActionId(StateP state, std::map<int, double> PA);
	std::vector<ClassifierP> generateActionSet(std::vector<ClassifierP> matchSet, int actionId);
	void updateActionSet(std::vector<ClassifierP> actionSet, DemeP deme, double reward, StateP state);
	void runGA(std::vector<ClassifierP> actionSet, GenotypeP input, DemeP deme, StateP state);

	void printPopulation();
	std::pair<int, double> getMaxFromPA(std::map<int, double> PA);
	std::set<int> getActionsFromMs (std::vector<ClassifierP> matchSet);
	double getAsTimeSum(std::vector<ClassifierP> as);
	
	void deleteFromPopulation(StateP state,  DemeP deme);
	void actionSetSubsumption(std::vector<ClassifierP> *actionSet, DemeP deme, StateP state);
	void removeFromPopSet(ClassifierP cl, DemeP deme);
};
typedef boost::shared_ptr<XCS> XCSP;

#endif

