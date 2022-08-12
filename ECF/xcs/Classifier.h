#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "../Algorithm.h"
#include "../bitstring/BitString.h"

#include "XCSParams.h"
#include "ClassifierParams.h"

#include <set>

class Classifier;
typedef boost::shared_ptr<Classifier> ClassifierP;

/**
* \ingroup xcs
* \brief Classifier class that holds all parameters 
* and pointer to individual to which the parameters belong.
*/
class Classifier {

public:

	IndividualP ind;
	ClassifierParamsP params;
	XCSParamsP xcsParams;

	Classifier(XCSParamsP xcsParams, unsigned long long int time, IndividualP ind, StateP state);
	Classifier (ClassifierP cl);
	static bool checkState(const StateP state);

	void cover (std::set<int> actions, const GenotypeP input,StateP state);
	bool doesMatch(const GenotypeP input);
	int getActionId();
	GenotypeP getAction();
	void setAction(GenotypeP action);

	void mutateRule(GenotypeP input, StateP state);
	void mutateAction(StateP state);

	bool valid;

	void print();
	//TODO std::string toString();

#pragma region BitString specific
	static void printBitString (const BitStringP bString);
	BitStringP getRuleBitString();
	BitStringP getDontCareBitString();
#pragma endregion

public:
	double getDeletionVote(double avFit);
	bool couldSubsume();
	int numOfDCBits();
	bool isMoreGeneral(ClassifierP cl);
	bool doesSubsume(ClassifierP cl);

	double getPrediction();
	double getError();
	double getFitness();

	unsigned long long int getTimeStamp();
	int getNumerosity();
	double getActSetSize();
	double getExperience();

	void setPrediction(double p);
	void setError(double eps);
	void setFitness(double F);

	void setTimeStamp(unsigned long long int ts);
	void setNumerosity(int num);
	void setActSetSize(double as);
	void setExperience(double exp);

private:
	void printRuleString (const BitStringP bString, const BitStringP hashString);
};


#endif
