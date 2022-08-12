#include "../ECF_base.h"
#include "../SelRandomOp.h"
#include "../SelFitnessProportionalOp.h"
#include "../SelWorstOp.h"
#include "../ECF_macro.h"

#include "AlgXCS.h"
#include "math.h"
#include "time.h"
#include "../Logger.h"
#include <string>
//
//#undef XCS_STEP_DEBUG
//#undef XCS_DEBUG

#pragma region Intro comments
//References:
//		[1] An algorithmic Description of XCS, Martin V. 
//			Butz and Stewart W. Wilson
//		[2] Rule-Based Evolutionary Online Learning Systems
//			Martin V. Butz

/*System characteristics:
	- single-step problems
	- multi-step problems
*/
#pragma endregion

//Compare function used for sorting classifier by fitness value
bool cmp (ClassifierP a, ClassifierP b){
	return a->ind->fitness->getValue() > b->ind->fitness->getValue();
}
using namespace std;

XCS::XCS()
{
	name_ = "XCS";
	
	//TODO: izbacit randomOp i worstOp
	selRandomOp = static_cast<SelectionOperatorP> (new SelRandomOp);
	selFitPropOp = static_cast<SelectionOperatorP> (new SelFitnessProportionalOp);
	selWorstOp = static_cast<SelectionOperatorP> (new SelWorstOp);

	params = static_cast<XCSParamsP> ( new XCSParams(name_));
}


//Method for registering algorithm parameters
void XCS::registerParameters(StateP state)
{
	params->registerParams(state->getRegistry());
	
	//Defining aditional genotype used for storing classifier parameters
	ClassifierParamsP params = static_cast<ClassifierParamsP> (new ClassifierParams(0,0,0));
	state->addGenotype(params);
	
}
bool XCS::initialize(StateP state)
{
	if (!Classifier::checkState(state)) {
		throw ("");
	}

    selRandomOp->initialize(state);
    selWorstOp->initialize(state);
	selFitPropOp->initialize(state);
	
	params->readParams(state->getRegistry());

	time = 0;

	ClassifierParamsP clParams = static_cast<ClassifierParamsP> (new ClassifierParams(0,0,0));
	clParams->setGenotypeId(3);
	clParams->initialize(state);
	params->initF_ = clParams->F_;

	//Environment initialization
	environment = boost::dynamic_pointer_cast<Environment> (evalOp_);
	
	if (!environment->checkState(state)) {
		throw ("");
	}

	try {
		if (!environment->initialize()){
			throw (std::string("failed to initialize"));
		}
	}catch (std::string text){
		ECF_LOG_ERROR(state, "Environment error: "+text);
		throw ("");
	}

    return true;
}
void XCS::printPopulation(){
	//sort(populationSet.begin(), populationSet.end(), cmp);
	for (uint i = 0; i < populationSet.size(); i++)
		populationSet[i]->print();
}

bool XCS::advanceGeneration(StateP state, DemeP deme) {	
	
	if (state->getGenerationNo() == 0){
		
		//creating population set [P]
		ClassifierP classP;
		PopulationP pop = state->getPopulation();

		for (uint i = 0; i < pop->size(); i++){
			for (uint j = 0; j < pop->at(i)->size(); j++){
				classP = static_cast<ClassifierP> 
					(new Classifier(params, time, pop->at(i)->at(j), state));
				populationSet.push_back(classP);
			}
		}

		for (uint i = 0; i < deme->size(); i++)
			deme->at(i)->fitness->setValue(params->initF_);
		environment->reset();
	}

	std::vector<ClassifierP> lastActionSet;  //[A]-1
	double lastReward = 0;
	GenotypeP lastInput;

	//main generation loop
	do {
		
#ifdef XCS_STEP_DEBUG
		std::cout << "Press any key to continue..." << std::endl;
		std::cin.get();
#endif
		time++;
		std::vector<ClassifierP> matchSet; //[M]
		std::vector<ClassifierP> actionSet;//[A]
	
		//Getting input from environment
		GenotypeP input = environment->getInput();

#ifdef XCS_DEBUG
		std::cout << "Input value: ";
		Classifier::printBitString(boost::dynamic_pointer_cast<BitString::BitString> (input));
		std::cout << std::endl;
#endif
		//Generate match set [M]
		matchSet = generateMatchSet(state, deme, input); //[M]
	
#ifdef XCS_DEBUG
		std::cout << "Match set [M]: "<< std::endl;
		for (uint i = 0; i < matchSet.size(); i++)
			matchSet[i]->print();
#endif
		
		//creating prediction array PA
		std::map<int, double> PA = generatePA(matchSet);	
		//selecting action id
		
		int actionId = selectActionId(state, PA);
#ifdef XCS_DEBUG
		std::cout << "action id = " << actionId<< std::endl;
#endif

		//generating action set [A]
		actionSet = generateActionSet(matchSet, actionId);

#ifdef XCS_DEBUG
		std::cout << "\nAction set [A]: "<< std::endl;
		for (uint i = 0; i < actionSet.size(); i++)
			actionSet[i]->print();
#endif

		//executing selected action
		IndividualP ind = static_cast<IndividualP> (new Individual);
		ind->push_back(input);
		ind->push_back(actionSet[0]->getAction());
		
		//getting reward from environment
		evaluate(ind);
		double reward = ind->fitness->getValue();

#ifdef XCS_DEBUG
		std::cout << "Reward: " << reward << std::endl;
#endif		
		
		if (!lastActionSet.empty()){
			double P = lastReward + params->gama_ * getMaxFromPA(PA).second;

			updateActionSet(lastActionSet, deme,  P, state);
			if (!environment->isExploit()) {
				runGA(lastActionSet, lastInput, deme, state);
			}
		}
		if (environment->isOver()) {
			
			if (!environment->isExploit()) {
				updateActionSet(actionSet, deme, reward, state);
				
				runGA(actionSet, input, deme, state);
				lastActionSet.clear();
			}

		} else {
			lastActionSet = actionSet;
			lastReward = reward;
			lastInput = input;
		}		
	} while (!environment->isOver());
		
	environment->nextTrial();

#ifdef XCS_DEBUG
	std::cout << "Classifiers:" << std::endl;
	printPopulation();
	std::cout << " ===== advanceGeneration end ====="<< std::endl;
#endif

	return true;
}

std::vector<ClassifierP> XCS::generateMatchSet(StateP state, DemeP deme, GenotypeP input) {

	//(!) napomena mora vrijediti deme->at(i) == vClassifier[i].ind
	std::vector<ClassifierP> matchSet;

	while(matchSet.empty()){

		for (uint i = 0; i < populationSet.size(); ++i){
			if (populationSet[i]->doesMatch(input))
				matchSet.push_back(populationSet[i]);
		}

		uint noDiffActions = (uint) getActionsFromMs(matchSet).size();
		if (noDiffActions < params->mna_){
				
			ClassifierP coverCl = cover(state, deme, input, matchSet);
			deleteFromPopulation(state, deme);
			matchSet.clear();
		}
	}

	return matchSet;
} 

std::set<int> XCS::getActionsFromMs (std::vector<ClassifierP> matchSet){

	std::set<int> actions;
	for (uint i = 0; i < matchSet.size(); ++i){
		actions.insert(matchSet[i]->getActionId());
	}
	return actions;
}

//Creates cover classifier according to input value
//and inserts it in populationSet and deme
ClassifierP XCS::cover (StateP state, DemeP deme, GenotypeP input, std::vector<ClassifierP> matchSet){
	
	IndividualP newInd = static_cast<IndividualP> (new Individual(state));
	
	ClassifierP newClassifier = static_cast<ClassifierP> (new 
		Classifier (params,time, newInd, state));
	
	//classifier must match input so that it can be added in [M]
	newClassifier->cover(getActionsFromMs(matchSet), input, state);
	newInd->index = (uint)deme->size();
	
	populationSet.push_back(newClassifier);
	deme->push_back(newInd);

		assert(deme->size() == populationSet.size());

	return newClassifier;
}

//Generates prediction array from match set
std::map<int, double> XCS::generatePA(std::vector<ClassifierP> matchSet){
	std::map<int, double> PA;
	std::map<int, double> fsa; //fitness sum array

		for (uint i = 0; i < matchSet.size(); ++i){

			ClassifierP cl = matchSet[i];
			double fitness = cl->getFitness();
			int action = cl->getActionId();
		
			if(PA[action] == NULL) {
				PA[action] = cl->getPrediction() * fitness;
				fsa[action] = 0;
			} else 
				PA[action] += cl->getPrediction() * fitness;
			fsa[action] += fitness;
		}
		for (std::map<int, double>::iterator it = PA.begin(); it != PA.end(); ++it){
			PA[it->first] /= (fsa[it->first] == 0 ? 1 : fsa[it->first]);
		}
	return PA;
}


int XCS:: selectActionId(StateP state, std::map<int, double> PA){
	double rnd = state->getRandomizer()->getRandomDouble();
	int actionId = PA.begin()->first;

	//if it is exploit experiment
	if (environment->isExploit())
		return getMaxFromPA(PA).first;

	if (rnd < params->p_explore_) {
		//exploration
		rnd = state->getRandomizer()->getRandomDouble();
		int i = 1;
		for (std::map<int, double>::const_iterator it = PA.begin(); it != PA.end(); ++it){
			if (i > PA.size() * rnd){
				actionId = it->first;
				break;
			}
			i++;
		}
	}else {
		//exploitation
		actionId = getMaxFromPA(PA).first;	
	}
	return actionId;
}

std::vector<ClassifierP> XCS::generateActionSet(std::vector<ClassifierP> matchSet, int actionId){
	std::vector<ClassifierP> actionSet;
	for (uint i = 0; i < matchSet.size(); i++)
		if (matchSet[i]->getActionId() == actionId)
			actionSet.push_back(matchSet[i]);
	return actionSet;
}

void XCS::updateActionSet(std::vector<ClassifierP> actionSet, DemeP deme, double reward, StateP state){
	
	ClassifierP cl;
	double sum = 0;
	std::vector<double> accuracy;
	
	int numSum = 0;
	for (uint i = 0; i < actionSet.size(); ++i)
		numSum += actionSet[i]->getNumerosity();

	//update exp, eps, p and as
	for (uint i = 0; i < actionSet.size(); ++i){
		cl = actionSet[i];
		
		double exp = cl->getExperience() + 1;
		cl->setExperience(exp);

		double eps = cl->getError();
		double p = cl->getPrediction();
		double as = cl->getActSetSize();

		if (exp < 1 / params->beta_) {
			p += (reward - p) / exp;
			eps += (fabs(reward - p) - eps) / exp;
			as += (numSum - as) / exp;
		} else {
			p += params->beta_ * (reward - p);
			eps += params->beta_ * (fabs(reward - p) - eps);
			as += params->beta_ * (numSum - as);
		}

		if (eps < params->eps0_)
			accuracy.push_back(1);
		else 
			accuracy.push_back( params->alpha_ * pow(eps / params->eps0_, -params->accExp_) );
		
		sum += accuracy[i] * cl->getNumerosity();

		cl->setPrediction(p);
		cl->setError(eps);
		cl->setActSetSize(as);
	}

	//update fitness
	for (uint i = 0; i < actionSet.size(); ++i){
		
		cl = actionSet[i];

		double F = cl->getFitness();
		F += params->beta_ * (accuracy[i] * cl->getNumerosity() / sum - F) ;
		cl->setFitness(F);
	}

	actionSetSubsumption(&actionSet, deme, state);
	
}

double XCS::getAsTimeSum(std::vector<ClassifierP> as) {

	double sumNum = 0, sumTsNum = 0;
	for (uint i = 0; i < as.size(); ++i){
		sumTsNum += as[i]->getTimeStamp() * as[i]->getNumerosity();
		sumNum += as[i]->getNumerosity();
	}
	
	return sumTsNum / sumNum;
}

void XCS::runGA(std::vector<ClassifierP> actionSet, GenotypeP genInput, DemeP deme, StateP state){

	if (time - getAsTimeSum(actionSet) < params->thresholdGA_) return;

	std::vector<IndividualP> tournament, vActionSet;

	for (uint i = 0; i < actionSet.size(); i++) {
		if (!actionSet[i]->valid) continue;
		vActionSet.push_back(actionSet[i]->ind);
		actionSet[i]->setTimeStamp(time);
	}
	if (vActionSet.size() < 1) return;

	IndividualP parent[2];
	parent[0] = selFitPropOp->select(vActionSet);
	parent[1] = selFitPropOp->select(vActionSet);

	ClassifierP clParent[2];
	clParent[0] = populationSet[parent[0]->index];
	clParent[1] = populationSet[parent[1]->index];

	assert(clParent[0]->getActionId() == clParent[1]->getActionId());
	ClassifierP clChild[2];

	for (int i = 0; i < 2; ++i) {

		clChild[i] = static_cast<ClassifierP> (new Classifier(clParent[i]));

		clChild[i]->setNumerosity(1);
		clChild[i]->setExperience(0);

		double f = clChild[i]->getFitness();

		if (state->getRandomizer()->getRandomDouble() < params->pCrossover_) {
			mate(parent[0], parent[1], clChild[i]->ind);
			
			clChild[i]->setAction(clParent[0]->getAction());

			assert(clChild[i]->getActionId() == clParent[i]->getActionId());

			clChild[i]->setPrediction((clParent[0]->getPrediction() + clParent[1]->getPrediction()) / 2);
			clChild[i]->setError((clParent[0]->getError() + clParent[1]->getError()) / 2);

			f = (clParent[0]->getFitness() + clParent[1]->getFitness()) / 2;
		}

		clChild[i]->setFitness( 0.1 * f);

		clChild[i]->mutateRule(genInput, state);
		clChild[i]->mutateAction(state);

		if (clParent[0]->doesSubsume(clChild[i])) {
			clParent[0]->setNumerosity(clParent[0]->getNumerosity() + 1);
		} else if (clParent[1]->doesSubsume(clChild[i])) {
			clParent[1]->setNumerosity(clParent[1]->getNumerosity() + 1);
		} else {
			clChild[i]->ind->index = (uint)deme->size();
			populationSet.push_back(clChild[i]);
			deme->push_back(clChild[i]->ind);
		}
			assert(deme->size() == populationSet.size());

		deleteFromPopulation(state, deme);
	}	
}

std::pair<int, double> XCS::getMaxFromPA(std::map<int, double> PA){
	
	if (PA.empty()) return std::make_pair(-1, -1.);

	std::pair<int, double> maxPA = *PA.begin();
	for (std::map<int, double>::iterator it = PA.begin(); it != PA.end(); ++it){
		if (it->second > maxPA.second)
			maxPA = *it;
	}
	return maxPA;
}

void XCS::deleteFromPopulation(StateP state, DemeP deme) {

	int sumNum = 0;
	double sumFit = 0;

	for ( uint i = 0; i < populationSet.size(); ++i) {
		sumNum += populationSet[i]->getNumerosity();
		sumFit += populationSet[i]->getFitness();
	}
	if (sumNum <= (int) params->popSize_) return;

	double avFitInPop = sumFit / sumNum;
	double sumVote = 0;

	for ( uint i = 0; i < populationSet.size(); ++i) {
		sumVote += populationSet[i]->getDeletionVote(avFitInPop);
	}
	double choicePoint = state->getRandomizer()->getRandomDouble() * sumVote;
	sumVote = 0;
	for ( uint i = 0; i < populationSet.size(); ++i) {
		ClassifierP cl = populationSet[i];
		sumVote += cl->getDeletionVote(avFitInPop);
		if (sumVote > choicePoint) {
			int n = cl->getNumerosity();
			if (n > 1){
				cl->setNumerosity(n-1);
			} else {
				removeFromPopSet(cl, deme);
			}
			return;
		}
	}
	assert(deme->size() == populationSet.size());

}

void XCS::removeFromPopSet(ClassifierP cl, DemeP deme){
	
	IndividualP lastInd = deme->at(deme->size()-1);
	ClassifierP lastCl = populationSet[populationSet.size()-1];

	lastInd->index = cl->ind->index;

	populationSet[cl->ind->index] = lastCl;
	populationSet.erase(populationSet.begin() + populationSet.size() - 1);

	deme->at(cl->ind->index) = lastInd;
	deme->erase(deme->begin() + deme->size() - 1);
	
	assert(deme->size() == populationSet.size());
	cl->valid = false;
}

void XCS::actionSetSubsumption(std::vector<ClassifierP> *actionSet, DemeP deme, StateP state){
	ClassifierP cl;
	bool clSet = false;
	
	for (uint i = 0; i < actionSet->size(); ++i){
		
		ClassifierP c = actionSet->at(i);
		if (c->couldSubsume()){

			int clDcb = c->numOfDCBits();
			double rnd = state->getRandomizer()->getRandomDouble();
			if (!clSet || clDcb > cl->numOfDCBits() || (clDcb == cl->numOfDCBits() && rnd < 0.5 )) {
				cl = c;
				clSet = true;
			}
		}
	}
	if (clSet) {
		for (uint i = 0; i < actionSet->size(); ++i){
			ClassifierP c = actionSet->at(i);
			if (cl->isMoreGeneral(c)){
				cl->setNumerosity(cl->getNumerosity() + c->getNumerosity());
				actionSet->erase(actionSet->begin() + i);
				removeFromPopSet(c,deme);
			}
		}
	}
}
