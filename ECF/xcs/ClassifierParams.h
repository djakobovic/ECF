#ifndef ClassifierParams_h
#define ClassifierParams_h

#include "../Genotype.h"
#include<sstream>

/**
* \ingroup xcs
* \brief Classifier data structure in XCS algorithm.
*/
class ClassifierParams : public Genotype {
private:

public:
	double p_; //reward prediction
	double eps_; //reward prediction error
	double F_; //fitness
	
	double as_; //action set size estimate
	unsigned long long int ts_; //time stamp
	double exp_; //experience
	int num_; //numerosity

	ClassifierParams(double p, double eps, double F) {	
		this->p_ = p;
		this->eps_ = eps;
		this->F_ = F;

		this->as_ = 1;
		this->num_ = 1;
		this->exp_ = 0;
		this->ts_ = 0;
		name_ = "ClassifierParams";
	}
	virtual ~ClassifierParams()
	{	}

	void registerParameters(StateP state)
	{
		registerParameter(state, "fitness", (voidP) new double(0), ECF::DOUBLE);
		registerParameter(state, "error", (voidP) new double(0), ECF::DOUBLE);
		registerParameter(state, "prediction", (voidP) new double(0), ECF::DOUBLE);

	}

	virtual bool initialize(StateP state) {
		
		voidP vp = getParameterValue(state, "fitness");
		F_ = *((double*) vp.get());
		vp = getParameterValue(state, "prediction");
		p_ = *((double*) vp.get());
		vp = getParameterValue(state, "error");
		eps_ = *((double*) vp.get());
		
		return true;
	}

	virtual Genotype* copy() {
		
		ClassifierParams *newObject = new ClassifierParams(*this);
		return newObject;
	}

	virtual std::vector<CrossoverOpP> getCrossoverOp() {
		std::vector<CrossoverOpP> vCrossOp;
		return vCrossOp;
	}

	virtual std::vector<MutationOpP> getMutationOp() {
		std::vector<MutationOpP> vMutOp;
		return vMutOp;
	}	

	virtual void read(XMLNode& node) {
		std::stringstream ss;
		ss << node.getAttribute("prediction");
		ss >> p_;
		
		ss.str("");
		ss << node.getAttribute("fitness");
		ss >> F_;

		ss.str("");
		ss << node.getAttribute("error");
		ss >> eps_;

		ss.str("");
		ss << node.getAttribute("as");
		ss >> as_;

		ss.str("");
		ss << node.getAttribute("num");
		ss >> num_;

		ss.str("");
		ss << node.getAttribute("exp");
		ss >> exp_;

		ss.str("");
		ss << node.getAttribute("ts");
		ss >> ts_;
	}

	virtual void write(XMLNode& node) {
		node = XMLNode::createXMLTopNode("ClassifierParams");
		
		std::stringstream sValue;
		sValue << p_;
		node.addAttribute("prediction", sValue.str().c_str());

		sValue.str("");
		sValue << F_;
		node.addAttribute("fitness", sValue.str().c_str());

		sValue.str("");
		sValue << eps_;
		node.addAttribute("error", sValue.str().c_str());

		sValue.str("");
		sValue << num_;
		node.addAttribute("num", sValue.str().c_str());

		sValue.str("");
		sValue << ts_;
		node.addAttribute("ts", sValue.str().c_str());

		sValue.str("");
		sValue << as_;
		node.addAttribute("as", sValue.str().c_str());

		sValue.str("");
		sValue << exp_;
		node.addAttribute("exp", sValue.str().c_str());
	}

};
typedef boost::shared_ptr<ClassifierParams> ClassifierParamsP;

#endif 