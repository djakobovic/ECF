#include "Cartesian.h"
#include <cctype>
#include <map>
#include <vector>


namespace Cartesian {


Cartesian::Cartesian(void)
{
	name_ = "Cartesian";
}


Cartesian::~Cartesian(void)
{}


Cartesian* Cartesian::copy()
{
	Cartesian *newObject = new Cartesian(*this);
	// no deep copy code needed

	return newObject;
}


std::vector<CrossoverOpP> Cartesian::getCrossoverOp()
{
	vector<CrossoverOpP> crxOps;
	crxOps.push_back((CrossoverOpP) (new CartesianCrxOnePoint));	// ok
	crxOps.push_back((CrossoverOpP) (new CartesianCrxHalfUniform));	// ok
	crxOps.push_back((CrossoverOpP) (new CartesianCrxUniform));		// ok
	return crxOps;
}


std::vector<MutationOpP> Cartesian::getMutationOp()
{
	vector<MutationOpP> mutOps;
////	mutOps.push_back((MutationOpP) (new CartesianMutOnePoint));	// treba popraviti
	mutOps.push_back((MutationOpP) (new CartesianMutNonSilent));	// ok
////	mutOps.push_back((MutationOpP) (new CartesianMutSilent));	// treba popraviti
	mutOps.push_back((MutationOpP) (new CartesianMutNewParameterLess));	// ok
	return mutOps;
}


void Cartesian::registerParameters(StateP state)
{
	registerParameter(state, "numoutputs", (voidP) (new uint(1)), ECF::UINT, "number of functional outputs (default: 1)");
	registerParameter(state, "numrows", (voidP) (new uint(1)), ECF::UINT, "number of rows (default: 1)");
	registerParameter(state, "numcols", (voidP) (new uint(10)), ECF::UINT, "number of columns (default: 10)");
	registerParameter(state, "levelsback", (voidP) (new uint(2)), ECF::UINT, "number of previous columns to be used as possible inputs (default: 2)");
	registerParameter(state, "numvariables", (voidP) (new uint(1)), ECF::UINT, "number of input variables (default: 1)");
	registerParameter(state, "functionset", (voidP) (new std::string), ECF::STRING, "set of functions to use (default: none)");
	//registerParameter(state, "constantset", (voidP)(new std::string), ECF::STRING, "set of input constants (default: none)");
}


bool Cartesian::initialize(StateP state)
{
	if (!isParameterDefined(state, "functionset")){
		ECF_LOG_ERROR(state, "Error: required parameters for CGP genotype not defined (functionset)!");
		throw("");
	}

	state_ = state;
	std::stringstream ss;
	std::string names,name;
	voidP sptr;

	// create and initialize the function set
	functionSet = static_cast<FunctionSetP> (new FunctionSet);
	functionSet->initialize(state_);

	uint number;
	//Simple parameters
	sptr = getParameterValue(state, "numvariables");
	number = *((uint*) sptr.get());
	if(number <= 0) {
		ECF_LOG_ERROR(state, "CGP initialization error: Number of variables is smaller than 1 or can not be parsed into a number.");
		return false;
	}
	nVariables = number;

	sptr = getParameterValue(state,"numoutputs");
	number = *((uint*) sptr.get());
	if(number <= 0) {
		ECF_LOG_ERROR(state, "CGP initialization error: Number of outputs is smaller than 1 or can not be parsed into a number.");
		return false;
	}
	nOutputs = number;

	sptr = getParameterValue(state,"numrows");
	number = *((uint*) sptr.get());
	if(number <= 0) {
		ECF_LOG_ERROR(state, "CGP initialization error: Number of rows is smaller than 1 or can not be parsed into a number.");
		return false;
	}
	nRows = number;

	sptr = getParameterValue(state,"numcols");
	number = *((uint*) sptr.get());
	if(number <= 0) {
		ECF_LOG_ERROR(state, "CGP initialization error: Number of columns is smaller than 1 or can not be parsed into a number.");
		return false;
	}
	nCols = number;

	sptr = getParameterValue(state,"levelsback");
	number = *((uint*) sptr.get());
	if(number <= 0) {
		ECF_LOG_ERROR(state, "CGP initialization error: Number of columns is smaller than 1 or can not be parsed into a number.");
		return false;
	}
	nLevelsBack = number;

	nConstants = 0;
	sptr = getParameterValue(state,"constantset");
	if (sptr) {
		names = *((std::string*)sptr.get());
		number = 0;
		ss.str("");
		ss.clear();
		ss << names;
		while (ss >> name) {
			++number;
		}
		nConstants = number;
	}
	nInputs = nConstants + nVariables;

	//Functionset parametri su malo složeniji
	sptr = getParameterValue(state, "functionset");
	names = *((std::string*) sptr.get());
	ss.str("");
	ss.clear();
	ss << names;
	name="";

	while(ss >> name) {
		functionSet->addFunction(name);
		nFunctions++;
	}

	std::map<std::string, FunctionP>::iterator it;
	for (it = functionSet->mFunctionSet.begin(); it != functionSet->mFunctionSet.end(); it++) {
		uint nArgs = it->second->getNumberOfArguments();
		if (nArgs > maxArity)
		{
			maxArity = nArgs;
		}
	}
	buildRandomGenome();

	return true;
}


void Cartesian::read(XMLNode &xCart)
{
	std::string s = xCart.getText(0);
	std::istringstream ss(s);
	std::string token;
	uint i = 0;
	while (getline(ss, token, ' '))
	{
		this->at(i++) = stoi(token);
	}
}


void Cartesian::write(XMLNode &xCart)
{
	xCart = XMLNode::createXMLTopNode("Cartesian");
	std::stringstream sValue;
	sValue << this->size();
	xCart.addAttribute("size", sValue.str().c_str());

	sValue.str("");

	// write genome to sValue
	std::vector<uint>& genome = *this;
	uint i = 0;
	do {
		uint iFunction = genome[i++];
		sValue << "(" << functionSet->vFunctions[iFunction]->getName() << " ";
		//for(uint arg = 0; arg < functionSet->vFunctions[iFunction]->getNumberOfArguments(); arg++)
		// workaround dok se genotip ne izradi kao vektor gena:
		for (uint arg = 0; arg < maxArity; arg++)
			sValue << genome[i++] << " ";
		sValue << ") ";
	} while (i < (genome.size() - nOutputs));
	for (uint out = 0; out < nOutputs; out++)
		sValue << genome[i++] << " ";

	xCart.addText(sValue.str().c_str());
}


uint Cartesian::getGenomeSize()
{
	return this->size();
}


void Cartesian::buildRandomGenome() 
{
	for (uint i = 0; i < nRows; i++) {
		for(uint j = 0; j < nCols; j++) {

			int functionID = state_->getRandomizer()->getRandomInteger(nFunctions);
			//Pushing a function
			this->push_back(functionID);
			std::map<std::string, FunctionP>::iterator it;
			FunctionP functionName = functionSet->vFunctions[functionID];
			//for(uint k = 0; k < functionName->getNumberOfArguments(); k++) {
			// workaround dok se genotip ne izradi kao vektor gena:
			for (uint k = 0; k < this->maxArity; k++) {
				this->push_back(randomConnectionGenerator(i));
			}

		}
	}
	for(uint i = 0; i < nOutputs; i++) {
		this->push_back(randomConnectionGenerator(nRows));
	}
}


uint Cartesian::randomConnectionGenerator(uint rowNumber) 
{
	//This is the first index which is actually limited by levels back
	int minimum = nInputs + nCols*nLevelsBack;

	//Index of the first element of a row
	int firstElementOfARow = nInputs + rowNumber*nCols;
	if(minimum <= firstElementOfARow) {
		minimum = firstElementOfARow - nLevelsBack*nCols;
	}
	else {
		minimum = 0;
	}
	return state_->getRandomizer()->getRandomInteger(minimum, firstElementOfARow - 1);
}


void Cartesian::evaluate(vector<double>& inputData, vector<double>& results) 
{
	vector<double> working_vector (inputData);
	working_vector.resize(this->nInputs);
	vector<double> operands(this->maxArity);
	double result = 0;
	for(uint i = 0; i < this->size() - nOutputs; i++) {
		int operatorID = this->operator[](i);
		int numberOfArguments = functionSet->vFunctions[operatorID]->getNumberOfArguments();
		//for(int k = i + 1; k < i + numberOfArguments + 1; k++) {
		// workaround dok se genotip ne izradi kao vektor gena:
		for (uint k = i + 1; k < i + maxArity + 1; k++) {
			operands[k - i - 1] = working_vector[this->operator[](k)];
		}
		functionSet->vFunctions[operatorID]->evaluate(operands, result);
		working_vector.push_back(result);
		result = 0;
		//i+=numberOfArguments;
		// workaround dok se genotip ne izradi kao vektor gena:
		i += maxArity;
	}
	results.resize(nOutputs);
	int ir = 0;
	for(uint i = this->size() - nOutputs; i < this->size(); i++) {
		results[ir++] = working_vector[this->operator[](i)];
	}
}


}





