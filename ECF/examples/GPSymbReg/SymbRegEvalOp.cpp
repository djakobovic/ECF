#include <cmath>
#include "../../ECF.h"
#include "SymbRegEvalOp.h"

void SymbRegEvalOp::registerParameters(StateP state)
{
	state->getRegistry()->registerEntry("domain", (voidP) (new std::string), ECF::STRING);
	state->getRegistry()->registerEntry("codomain", (voidP) (new std::string), ECF::STRING);
}

bool SymbRegEvalOp::initialize(StateP state)
{
	domain.clear();
	codomain.clear();

/*
	double datum;
	std::stringstream ss;

	// check if the parameters are defined in the conf. file
	// if not, we return false so the initialization fails
	if(!state->getRegistry()->isModified("domain") ||
		!state->getRegistry()->isModified("codomain"))
		return false;

	voidP sptr = state->getRegistry()->getEntry("domain"); // get parameter value
	ss << *((std::string*) sptr.get()); // convert from voidP to user defined type
	while(ss >> datum) {	// read all the data from string
		domain.push_back(datum);
	}
	ss.str("");
	ss.clear();	// reset stringstream object
	sptr = state->getRegistry()->getEntry("codomain");	// get codomain parameter
	ss << *((std::string*) sptr.get());
	while(ss >> datum) {	// read values
		codomain.push_back(datum);
	}

	if(domain.size() != codomain.size())	// if the parameters are ill defined, return false
		return false;
	nSamples = (uint) domain.size();

	return true;
*/

	nSamples = 20;
	double x = -10;
	for(uint i = 0; i < nSamples; i++) {
		domain.push_back(x);
		//codomain.push_back(x + sin(x));
		codomain.push_back(x*x*x*x*x - 2*x*x*x*x + 3*x*x*x - 2*x*x + sin(x));
		x += 1;
	}
	return true;
}

struct my_type
{
	double v;
	bool b;
};

FitnessP SymbRegEvalOp::evaluate(IndividualP individual)
{
	// stvaranje zeljenog Fintess objekta:
	FitnessP fitness = static_cast<FitnessP> (new FitnessMin);

	// dohvat genotipa jedinke
	TreeP tree = boost::dynamic_pointer_cast<Tree::Tree> (individual->getGenotype());
	//Tree* tree = (Tree*) individual->at(0).get();

	double value = 0;
	for(uint i = 0; i < nSamples; i++) {

		tree->setTerminalValue("X", &domain[i]);
		/*
		for(uint term = 0; term < tree->primitiveSet_->terminalSet.size(); term++) 
			tree->primitiveSet_->terminalSet[term]->value = domain[i];
		*/
		double result;
		tree->execute(&result);
		value += fabs(codomain[i] - result);
	}
	fitness->setValue(value);

	// primjer koristenja vlastitog tipa podatka za terminale
	//my_type x;
	//x.b = true;
	//x.v = false;
	//tree->setTerminalValue("term", &x);
	//tree->execute(&x);
	//fitness->setValue(x.v);


	return fitness;
}