#include "../ECF_base.h"
#include "../FitnessMax.h"
#include "../ECF_macro.h"

#include "Classifier.h"
#include "time.h"

//Class that represents single classifier in XCS system
//This implementation is designed for BitString::BitString genotypes for
//rule, input and action definition
//By extending it, other genotypes could also be used.
//All used genotypes need to have the same type as corresponding in configuration file.

Classifier::Classifier(XCSParamsP xcsParams, unsigned long long int time, IndividualP ind, StateP state){

	this->xcsParams = xcsParams;
	params = boost::dynamic_pointer_cast<ClassifierParams> (ind->getGenotype(3));

	this->ind = ind;
	this->ind->fitness = static_cast<FitnessP> (new FitnessMax);
	setFitness(xcsParams->initF_);
	
	setTimeStamp(time);

	BitStringP dontCareBits = getDontCareBitString();
	for (uint i = 0; i < dontCareBits->bits.size(); ++i){
		double rnd = state->getRandomizer()->getRandomDouble();
		if (rnd < xcsParams->pdc_) 
			dontCareBits->bits[i] = true;
		else
			dontCareBits->bits[i] = false;
	}
	valid = true;
}

Classifier::Classifier(ClassifierP cl){

	xcsParams = cl->xcsParams;
	ind = static_cast<IndividualP> (cl->ind->copy());
	params = boost::dynamic_pointer_cast<ClassifierParams> (ind->getGenotype(3));
	valid = true;
}

//Checks if current state configuration is acceptable by Classifier
bool Classifier::checkState(const StateP state) {

	// minimum 3 genotypes required
	if(state->getGenotypes().size() < 3) {
		ECF_LOG_ERROR(state, "Error: XCS algorithm requires min. 3 genotypes!");
		return false;
	}
	
	BitStringP bitstring = static_cast<BitStringP> (new BitString::BitString);

	if(state->getGenotypes()[0]->getName() != bitstring->getName() 
		|| state->getGenotypes()[1]->getName() != bitstring->getName()) {
			ECF_LOG_ERROR(state, "Error: XCS algorithm accepts only BitString::BitString genotype as input!");
			return false;
	}

	return true;
}

//Checks if classifier matches input value
bool Classifier::doesMatch(const GenotypeP inp) {
	
	BitStringP input = boost::static_pointer_cast<BitString::BitString> (inp);

	BitStringP ruleBits = boost::static_pointer_cast<BitString::BitString> (ind->getGenotype(0));
	BitStringP dontCareBits = boost::static_pointer_cast<BitString::BitString> (ind->getGenotype(1));	
	
	for (uint i = 0; i < input->bits.size(); i++){
		if (dontCareBits->bits[i] ) continue;
		if (ruleBits->bits[i] != input->bits[i]) return false;
	}
	return true;
}
//Fits classifier rule to match the input value
void Classifier::cover (std::set<int> actions, const GenotypeP input, StateP state){
	ind->at(0) = static_cast<GenotypeP> (input->copy());
	BitStringP action = boost::dynamic_pointer_cast<BitString::BitString> (getAction());

	do {
		action->initialize(state);
	} while (actions.find(getActionId()) != actions.end());

}

//Prints classifier data on screen
void Classifier::print(){
	
	std::cout << "	[" << ind->index << "] ";
	printRuleString (getRuleBitString(),getDontCareBitString());
	std::cout << " : ";
	
	printBitString (boost::dynamic_pointer_cast<BitString::BitString> (getAction()) );
	
	std::cout << " ("<< getError() << ", " << getPrediction() <<", "<< getFitness() << ")";
	std::cout << "\t[ " << getNumerosity() << ", " << getActSetSize() << ", " << getExperience() << ", " << getTimeStamp() << " ]";
	std::cout << std::endl;
}

void Classifier::printRuleString (const BitStringP bString, const BitStringP hashString) {
	char sign;
	
	for (uint i = 0; i < bString->bits.size(); i++){
		if (hashString != NULL && hashString->bits[i]) sign = '#';
		else sign = bString->bits[i] ? '1':'0';
		std::cout << sign;
		
	}
}

void Classifier::printBitString (const BitStringP bString) {
	
	for (uint i = 0; i < bString->bits.size(); i++){
		std::cout << bString->bits[i] ? '1':'0';
	}
	
}

//Each action has id that is used for creating PA
//Since action is Genotype XCS can't make a difference
//beetwean two abstract classes
int Classifier::getActionId(){
	
	int ret = 0;
	BitStringP bstring = boost::dynamic_pointer_cast<BitString::BitString> (ind->getGenotype(2));
	/* Promjenjeno zbog mux problema
	if (bstring->bits[0]) ret += 1;
	if (bstring->bits[1]) ret += 10;
	if (bstring->bits[2]) ret += 100;
	*/

	int add = 1;
	for (uint i =0; i < bstring->bits.size(); ++i){
		if (bstring->bits[i]) ret += add;
		add *= 10;
	}

	return ret;
}

GenotypeP Classifier::getAction(){
	return ind->getGenotype(2);
}
void Classifier::setAction(GenotypeP action){
	ind->at(2) = static_cast<GenotypeP> (action->copy());
}


#pragma region Parameters setters

void Classifier::setError(double eps){
	params->eps_ = eps;
}

void Classifier::setPrediction(double p){
	params->p_ = p;
}

void Classifier::setFitness(double F){
	ind->fitness->setValue(F);
}

void Classifier::setTimeStamp(unsigned long long int ts) {
	params->ts_ = ts;
}

void Classifier::setNumerosity(int num) {
	params->num_ = num;
}

void Classifier::setActSetSize(double as) {
	params->as_ = as;
}

void Classifier::setExperience(double exp) {
	params->exp_ = exp;
}

#pragma endregion

#pragma region Parameters getters

double Classifier::getError(){
	return params->eps_;
}

double Classifier::getPrediction(){
	return params->p_;
}

double Classifier::getFitness(){
	return ind->fitness->getValue();
}

unsigned long long int Classifier::getTimeStamp() {
	return params->ts_;
}

int Classifier::getNumerosity(){
	return params->num_;
}

double Classifier::getActSetSize(){
	return params->as_;
}

double Classifier::getExperience(){
	return params->exp_;
}

#pragma endregion

double Classifier::getDeletionVote(double avFit){
	double vote = getActSetSize() * getNumerosity();
	if (getExperience() > xcsParams->thresholdDel_ && 
		getFitness() / getNumerosity() < xcsParams->delta_ * avFit){
			vote = vote * avFit / (getFitness() / getNumerosity());
	}
	return vote;
}
bool Classifier::couldSubsume(){
	double exp = getExperience();
	double eps = getError();
	if (exp > xcsParams->thresholdSub_ && eps < xcsParams->eps0_){
		return true;
	}
	return false;
}

int Classifier::numOfDCBits(){
	int num = 0;
	BitStringP dcbstring = getDontCareBitString();
	for (uint i = 0; i < dcbstring->bits.size(); ++i){
		if (dcbstring->bits[i]) ++num;
	}
	return num;
}
bool Classifier::isMoreGeneral(ClassifierP cl){
	if (numOfDCBits() <= cl->numOfDCBits()) return false;

	BitStringP dcGen = getDontCareBitString();
	BitStringP ruleGen = getRuleBitString();
	BitStringP ruleSpec = cl->getRuleBitString();
	int bitsSize = (int) ruleGen->bits.size();

	int i = 0;
	do {
		if (!dcGen->bits[i] && ruleGen->bits[i] != ruleSpec->bits[i])
			return false;
		++i;
	} while(i < bitsSize);

	return true;
}

bool Classifier::doesSubsume(ClassifierP cl){

	if (cl->getActionId() == getActionId()){
		if (couldSubsume()){
			if (isMoreGeneral(cl)){
				return true;
			}
		}
	}
	return false;
}


BitStringP Classifier::getRuleBitString(){
	return boost::dynamic_pointer_cast<BitString::BitString> (ind->getGenotype(0));
}

BitStringP Classifier::getDontCareBitString(){
	return boost::dynamic_pointer_cast<BitString::BitString> (ind->getGenotype(1));
}

void Classifier::mutateRule(GenotypeP genInput, StateP state) {

	double rnd = state->getRandomizer()->getRandomDouble();
	BitStringP input = boost::dynamic_pointer_cast<BitString::BitString> (genInput);

	for (uint i= 0; i < input->bits.size(); i++){
		if (rnd < xcsParams->pMutation_){
			BitStringP ruleBits = getRuleBitString();
			BitStringP dontCareBits = getDontCareBitString();
			
			if (dontCareBits->bits[i] == true){
				dontCareBits->bits[i] = false;
				ruleBits->bits[i] = input->bits[i];
			} else 
				dontCareBits->bits[i] = true;
		}
	}
}

void Classifier::mutateAction(StateP state) {
		double rnd = state->getRandomizer()->getRandomDouble();
		if (rnd < xcsParams->pMutation_){
			BitStringP actionBits = boost::dynamic_pointer_cast<BitString::BitString>  (getAction());
			actionBits->initialize(state);
		}
}