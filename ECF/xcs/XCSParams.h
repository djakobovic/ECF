#ifndef XCSParams_h
#define XCSParams_h

#include "../ECF_base.h"
//#include "../XML/xmlParser.h"
//#include "../Registry.h"

/**
* \ingroup xcs
* \brief Parameters for the XCS algorithm
*/
class XCSParams {
private:
	std::string name_;
	voidP getParameterValue(RegistryP registry, std::string name)
	{
		return registry->getEntry(name_ + "." + name);
	}
	bool registerParameter(RegistryP registry, std::string name, voidP value, ECF::type T)
	{
		return registry->registerEntry(name_ + "." + name, value, T);
	}
public:
	uint mna_; //minimum number of different actions in match set [M]
	
	double initF_; //initial fitness value
	double beta_; //learning rate
	double gama_; //discount factor used in multi-step problems
	double p_explore_; //probability of choosing random action

	uint thresholdGA_; //after how many generations should GA be runned
	double thresholdDel_;
	double thresholdSub_;

	double eps0_; //threshold prediction error
	double alpha_; //classifier accuracy diferention parameter
	double accExp_; //accuracy exponent, used when calculating prediction error
	double pdc_; //probability of # appearence in classifier rule

	double pCrossover_; //probability of applying crossover in the GA
	double pMutation_; //probability of applying mutation in the GA

	double delta_;
	uint popSize_;

	XCSParams (std::string name) {name_ = name;}

	void registerParams(RegistryP registry);
	void readParams(RegistryP registry);
	XMLNode getXMLNode();

};
typedef boost::shared_ptr<XCSParams> XCSParamsP;

#endif