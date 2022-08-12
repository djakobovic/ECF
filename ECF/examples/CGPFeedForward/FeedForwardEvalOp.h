#ifndef FeedForwardEvalOp_h
#define FeedForwardEvalOp_h
#include <cmath>
#include "../../ECF.h"
#include "../../cartesian/Cartesian.h"
using namespace std;

namespace cart {

template <class T>
class FeedForwardEvalOp : public EvaluateOp
{
public:
	FitnessP evaluate(IndividualP individual);
	void registerParameters(StateP);
	bool initialize(StateP);
protected:
	vector<vector<T>> domain;
	vector<vector<T>> codomain;
	uint inputs;
	uint vars;
	uint outputs;
	CartesianP car_;

	/**
	Evaluate group of elements from domain (on position inputSet in domain vector) through genotype.
	Return results produced by initial inputs (their original values) and produced by output of 
	every node in genotype.
	*/
	vector<T> evaluate(int inputSet);
};

//TODO: implementirati evaluaciju i za bool vrijednosti
typedef FeedForwardEvalOp<double> FeedForwardEvalOpDouble;
typedef FeedForwardEvalOp<int> FeedForwardEvalOpInt;
typedef boost::shared_ptr<FeedForwardEvalOpDouble> FeedForwardEvalOpDoubleP;
typedef boost::shared_ptr<FeedForwardEvalOpInt> FeedForwardEvalOpIntP;

template <class T>
void FeedForwardEvalOp<T>::registerParameters(StateP state)
{
	state->getRegistry()->registerEntry("domain", (voidP) (new string), ECF::STRING);
	state->getRegistry()->registerEntry("codomain", (voidP) (new string), ECF::STRING);
}

template <class T>
bool FeedForwardEvalOp<T>::initialize(StateP state)
{
	T value;
	vector<T> subset;
	std::stringstream ss;

	if (!state->getRegistry()->isModified("domain") || !state->getRegistry()->isModified("codomain"))
	{
		return false;
	}

	//fetch some individual to extract genotype - we need number of inputs and outputs of genotype
	cart::CartesianP car = boost::dynamic_pointer_cast<cart::Cartesian>(state->getPopulation()->getLocalDeme()->at(0)->getGenotype());

	//number of initial inputs (number of variables + number of constants)
	inputs = car->getNumOfInputs();
	//number of variables = they will be replaced by elements of domain
	vars = car->getNumVars();
	//number of final outputs = they will be compared to elements of codomain
	outputs = car->getNumOfOutputs();

	//group of elements from domain will be replaced by all variables through evaluation
	voidP sptr = state->getRegistry()->getEntry("domain"); 
	ss << *((string*) sptr.get()); 
	while (ss >> value) 
	{	
		subset.push_back(value);
		if (subset.size() == vars)
		{
			domain.push_back(subset);
			subset.clear();
		}
	}

	//every group of elements from domain must be the size equal to number of variables
	for (int i = 0; i < (int)domain.size(); i++)
	{
		if (domain.at(i).size() != vars)
		{
			cerr << "Error: Domain is not correctly defined!" << endl;
			return false;
		}
	}

	//group of elements from codomain will be compared with all outputs through evaluation
	ss.str("");
	ss.clear();	
	subset.clear();
	sptr = state->getRegistry()->getEntry("codomain");	
	ss << *((string*) sptr.get());
	while (ss >> value) 
	{	
		subset.push_back(value);
		if (subset.size() == outputs)
		{
			codomain.push_back(subset);
			subset.clear();
		}
	}

	//every group of elements from codomain must be the size equal to number of outputs
	for (int i = 0; i < (int)codomain.size(); i++)
	{
		if (codomain.at(i).size() != outputs)
		{
			cerr << "Error: Codomain is not correctly defined!" << endl;
			return false;
		}
	}

	if (domain.size() != codomain.size())
	{
		cerr << "Error: Domain and codomain are not equal in sizes!" << endl;
		return false;
	}

	return true;
}

template <class T>
FitnessP FeedForwardEvalOp<T>::evaluate(IndividualP individual)
{
	FitnessP fitness = static_cast<FitnessP> (new FitnessMax);

	//fetch genotype
	car_ = boost::dynamic_pointer_cast<Cartesian> (individual->getGenotype());

	double value = 0;
	vector<T> results;

	uint outputstart = car_->getNumOfRows() * car_->getNumOfCols() * (car_->getNumOfInputConn() + 1);
	//for every group of elements from domain feed forward through genotype and check if results placed
	//on outputs are equal to the group of corresponding codomain elements
	for (int i = 0; i < (int) domain.size(); i++)
	{
		//fetch outputs for current elements of domain
		results = evaluate(i);
		for (int j = 0; j < (int)codomain.at(i).size(); j++)
		{
			//compare results with elements from codomain
			if (codomain.at(i).at(j) == results.at(car_->at(outputstart + j)))
			{
				++value;
			}
		}
	}

	//fitness value is number of results equal to outputs
	fitness->setValue(value);

	return fitness;
}

template <class T>
vector<T> FeedForwardEvalOp<T>::evaluate(int inputSet)
{
	//in results will be all input connections that can be connected to a node (meaning initial inputs
	//and output of every node)
	vector<T> results;
	//store variables replaced by elements of domain - initial inputs
	results = domain.at(inputSet);

	//the rest of initial inputs are constants (if there are any)
	voidP sptr = car_->getConstantNames();
	stringstream ss;
	T constant;
	ss << *((string*) sptr.get());
	while (ss >> constant)
	{
		results.push_back(constant);
	}

	vector<T> inputs;
	//evaluate node by node and fetch result from node function for given inputs, store it in results
	for (int i = 0; i < (int)(car_->getNumOfRows() * car_->getNumOfCols() * (car_->getNumOfInputConn() + 1)); i++)
	{
		inputs.clear();
		string save = "";
		//fetch all input connections that are entering in current node, replace them by elements from 
		//results vector (depending on the value from genotype)
		for (int j = 0; j < (int)car_->getNumOfInputConn(); j++)
		{
			stringstream ss;

			//this means if input connection of node is e.g. 4, element on position 4 in vector results 
			//is fetched and placed in inputs
			inputs.push_back(results.at(car_->at(i + j)));
			ss << results.at(car_->at(i + j));
			save += ss.str() + " ";
		}

		//save input connections in void pointer
		string *s = new string(save);
		voidP sendInputs = (voidP)s;
		T result;
		
		i = i + car_->getNumOfInputConn();
		//fetch result as output of current node by implemented function in current node
		car_->evaluate(sendInputs, &result, car_->at(i));

		//remember all results fetched
		results.push_back(result);

		//results.push_back(i);
	}

	return results;
}

}

#endif // FeedForwardEvalOp_h