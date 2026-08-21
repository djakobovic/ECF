#include <cctype>
#include <map>
#include <vector>
#include "Cartesian.h"

// crossover operators
#include "CartesianCrxOnePoint.h"
#include "CartesianCrxHalfUniform.h"
#include "CartesianCrxUniform.h"

// mutation operators
#include "CartesianMutOnePoint.h"
#include "CartesianMutNonSilent.h"
#include "CartesianMutOnePointActive.h"


namespace Cartesian {


Cartesian::Cartesian(void)
{
	name_ = "Cartesian";
	functionSet_ = (FunctionSetP)(new FunctionSet);
}


Cartesian::~Cartesian(void)
{}


Cartesian* Cartesian::copy()
{
	Cartesian *newObject = new Cartesian(*this);
	return newObject;
}


std::vector<CrossoverOpP> Cartesian::getCrossoverOp()
{
	std::vector<CrossoverOpP> crxOps;
	crxOps.push_back((CrossoverOpP) (new CartesianCrxOnePoint));	// ok
	crxOps.push_back((CrossoverOpP) (new CartesianCrxHalfUniform));	// ok
	crxOps.push_back((CrossoverOpP) (new CartesianCrxUniform));		// ok
	return crxOps;
}


std::vector<MutationOpP> Cartesian::getMutationOp()
{
	std::vector<MutationOpP> mutOps;
	mutOps.push_back((MutationOpP) (new CartesianMutOnePoint));
	mutOps.push_back((MutationOpP) (new CartesianMutNonSilent));	
	mutOps.push_back((MutationOpP) (new CartesianMutOnePointActive));	
	return mutOps;
}


void Cartesian::registerParameters(StateP state)
{
	registerParameter(state, "numoutputs", (voidP) (new uint(1)), ECF::UINT, "number of functional outputs (default: 1)");
	registerParameter(state, "numrows", (voidP) (new uint(1)), ECF::UINT, "number of rows (default: 1)");
	registerParameter(state, "numcols", (voidP) (new uint(10)), ECF::UINT, "number of columns (default: 10)");
	registerParameter(state, "levelsback", (voidP) (new uint(2)), ECF::UINT, "number of previous columns to be used as possible inputs (default: 2)");
	registerParameter(state, "numvariables", (voidP) (new uint(1)), ECF::UINT, "number of input variables (default: 1)");
	registerParameter(state, "functionset", (voidP) (new std::string), ECF::STRING, "set of functions to use (mandatory)");
	registerParameter(state, "constantset", (voidP)(new std::string), ECF::STRING, "set of input constants (default: none)");
}


bool Cartesian::initialize(StateP state)
{
	state_ = state;
	std::stringstream ss;
	std::string names,name;
	voidP sptr;

	sptr = getParameterValue(state, "functionset");
	names = *((std::string*)sptr.get());

	if (!isParameterDefined(state, "functionset")) {
		ECF_LOG_ERROR(state, "Cartesian initialization error: required parameters for CGP genotype not defined (functionset)!");
		return false;
	}

	// create and initialize the function set
	//functionSet_ = FunctionSetP (new FunctionSet);	// TODO: vratiti ako se FunctionSet prebaci u State
	functionSet_->initialize(state_);

	uint number;
	// Simple parameters
	sptr = getParameterValue(state, "numvariables");
	number = *((uint*) sptr.get());
	if(number <= 0) {
		ECF_LOG_ERROR(state, "Cartesian initialization error: Number of variables is smaller than 1 or cannot be parsed into a number.");
		return false;
	}
	nVariables_ = number;

	sptr = getParameterValue(state,"numoutputs");
	number = *((uint*) sptr.get());
	if(number <= 0) {
		ECF_LOG_ERROR(state, "Cartesian initialization error: Number of outputs is smaller than 1 or cannot be parsed into a number.");
		return false;
	}
	nOutputs_ = number;

	sptr = getParameterValue(state,"numrows");
	number = *((uint*) sptr.get());
	if(number <= 0) {
		ECF_LOG_ERROR(state, "Cartesian initialization error: Number of rows is smaller than 1 or cannot be parsed into a number.");
		return false;
	}
	nRows_ = number;

	sptr = getParameterValue(state,"numcols");
	number = *((uint*) sptr.get());
	if(number <= 0) {
		ECF_LOG_ERROR(state, "Cartesian initialization error: Number of columns is smaller than 1 or cannot be parsed into a number.");
		return false;
	}
	nCols_ = number;

	sptr = getParameterValue(state,"levelsback");
	number = *((uint*) sptr.get());
	if(number <= 0) {
		ECF_LOG_ERROR(state, "Cartesian initialization error: Number of columns is smaller than 1 or cannot be parsed into a number.");
		return false;
	}
	nLevelsBack_ = number;

	nConstants_ = 0;
	constants_.clear();
	sptr = getParameterValue(state,"constantset");
	if (sptr) {
		names = *((std::string*)sptr.get());
		ss.str("");
		ss.clear();
		ss << names;
		while (ss >> name) {
			std::istringstream token(name);
			double value;
			if (token >> value) {
				constants_.push_back(value);
			}
		}
		nConstants_ = (uint) constants_.size();
	}
	nInputs_ = nConstants_ + nVariables_;

	// Functionset parameters
	sptr = getParameterValue(state, "functionset");
	names = *((std::string*) sptr.get());
	ss.str("");
	ss.clear();
	ss << names;
	name="";

	nFunctions_ = 0;
	while(ss >> name) {
		if (functionSet_->addFunction(name))
			nFunctions_++;
		else {
			ECF_LOG_ERROR(state, "Cartesian initialization error: unknown function (\"" + name + "\")");
			return false;
		}
	}
	if (nFunctions_ == 0) {
		ECF_LOG_ERROR(state, "Cartesian initialization error: no valid functions in functionset");
		return false;
	}

	maxArity_ = 0;
	std::map<std::string, uint>::iterator it;
	for (it = functionSet_->mFunctionSet.begin(); it != functionSet_->mFunctionSet.end(); it++) {
		uint nArgs = functionSet_->vFunctions[it->second]->getNumberOfArguments();
		if (nArgs > maxArity_)
			maxArity_ = nArgs;
	}

	buildRandomGenome();

	return true;
}


void Cartesian::read(XMLNode &xCart)
{
	std::string s = xCart.getText(0);
	std::istringstream ss(s);
	std::string token;
	uint iNode = nInputs_;	// starting node index
	uint iOutput = 0;
	FunctionP func;
	while (ss >> token)
	{
		if (token[0] != '(') {	// output connections
			outputs_[iOutput++] = stoi(token);
			continue;
		}

		token.erase(0, 1);	// erase opening (
		std::map <std::string, unsigned int>::iterator it = functionSet_->mFunctionSet.find(token);
		if (it == functionSet_->mFunctionSet.end()) {
			ECF_LOG_ERROR(state_, "Cartesian genotype: unused function (" + token + ")!");
			throw std::runtime_error("Cartesian genotype: unused function (" + token + ")!");
		}

		nodes_[iNode].setPrimitive(functionSet_->vFunctions[it->second]);

		uint nArgs = functionSet_->vFunctions[it->second]->getNumberOfArguments();
		nodes_[iNode].arguments_.resize(nArgs);
		
		for (uint arg = 0; arg < nArgs; arg++) {
			ss >> token;
			nodes_[iNode].arguments_[arg] = stoi(token);
		}

		iNode++;
	}
}


void Cartesian::write(XMLNode &xCart)
{
	xCart = XMLNode::createXMLTopNode("Cartesian");
	std::stringstream sValue;
	sValue << getGenomeSize();
	xCart.addAttribute("size", sValue.str().c_str());

	sValue.str("");

	// output all nodes
	for (uint iNode = nInputs_; iNode < nodes_.size(); iNode++) {
		sValue << "(" << nodes_[iNode].primitive_->getName() << " ";
		uint nArgs = (uint) nodes_[iNode].arguments_.size();
		for (uint i = 0; i < nArgs; i++) {
			sValue << nodes_[iNode].arguments_[i];
			if (i < nArgs - 1)
				sValue << " ";
		}
		sValue << ") ";
	}
	for (uint i = 0; i < outputs_.size(); i++)
		sValue << outputs_[i] << " ";

	xCart.addText(sValue.str().c_str());
}


uint Cartesian::getGenomeSize()
{
	return nRows_ * nCols_ + nOutputs_;
}


void Cartesian::buildRandomGenome() 
{
	// allocate for input nodes
	nodes_.resize(nInputs_);

	for (uint iCol = 0; iCol < nCols_; iCol++) {
		for(uint j = 0; j < nRows_; j++) {

			Node newNode;

			// select a function
			int functionID = state_->getRandomizer()->getRandomInteger(nFunctions_);
			newNode.setPrimitive(functionSet_->vFunctions[functionID]);

			// select arguments
			FunctionP function = functionSet_->vFunctions[functionID];
			for (uint k = 0; k < function->getNumberOfArguments(); k++) {
				uint iArgument = randomNodeInputConnection(iCol);
				newNode.arguments_.push_back(iArgument);
			}
			nodes_.push_back(newNode);
		}
	}

	// select outputs (among all function nodes)
	outputs_.resize(nOutputs_);
	for(uint i = 0; i < nOutputs_; i++) {
		outputs_[i] = randomOutputConnection();
	}
}


uint Cartesian::randomNodeInputConnection(uint column)
{
	// First node index in this column
	uint firstNodeInColumn = nInputs_ + column * nRows_;

	// First column that is allowed
	uint firstAllowedColumn = (column > nLevelsBack_) ? column - nLevelsBack_ : 0;

	uint firstAllowedNode = nInputs_ + firstAllowedColumn * nRows_;

	uint nPreviousFunctionNodes = firstNodeInColumn - firstAllowedNode;

	uint nCandidates = nInputs_ + nPreviousFunctionNodes;

	uint id = state_->getRandomizer()->getRandomInteger(nCandidates);

	// primary inputs 
	if (id < nInputs_)
		return id;

	// allowed function nodes
	return firstAllowedNode + (id - nInputs_);
}


uint Cartesian::randomOutputConnection()
{
	return (uint) state_->getRandomizer()->getRandomInteger(nInputs_, (uint) nodes_.size() - 1);
}


void Cartesian::evaluate(std::vector<double>& inputData, std::vector<double>& results) 
{
	if (inputData.size() != nVariables_) {
		throw std::runtime_error("Cartesian error: inputData vector size not equal to number of input variables.");
	}

	std::vector<double> node_values(inputData);
	// add constants to values vector
	node_values.insert(node_values.end(), constants_.begin(), constants_.end());

	std::vector<double> operands(this->maxArity_);
	double result = 0;
	// calculate 
	for (uint i = nInputs_; i < nodes_.size(); i++) {
		int numberOfArguments = nodes_[i].primitive_->getNumberOfArguments();
		for(int k = 0; k < numberOfArguments; k++) {
			operands[k] = node_values[nodes_[i].arguments_[k]];
		}
		nodes_[i].primitive_->evaluate(operands, result);
		node_values.push_back(result);
	}
	results.resize(nOutputs_);
	int ir = 0;
	for (uint i = 0; i < outputs_.size(); i++) {
		results[i] = node_values[outputs_[i]];
	}
}


void Cartesian::getActiveFunctionNodes(std::vector<uint>& activeNodes)
{
	const uint size = (uint) nodes_.size();
	std::vector<bool> activeFlags(size, false);

	// for all output nodes, recursively add their argument nodes
	for (const uint i : outputs_)
		if (i >= nInputs_) {
			activeFlags[i] = true;
			addRecursivelyActiveFunctionNodes(activeFlags, i);
		}

	activeNodes.clear();
	for (uint i = nInputs_; i < activeFlags.size(); i++)
		if(activeFlags[i])
			activeNodes.push_back(i);
}


void Cartesian::addRecursivelyActiveFunctionNodes(std::vector<bool>& activeFlags, uint node)
{
	for (uint iArg = 0; iArg < nodes_[node].primitive_->getNumberOfArguments(); iArg++) {
		uint iActive = nodes_[node].arguments_[iArg];

		if (iActive >= nInputs_ && activeFlags[iActive] == false) {
			activeFlags[iActive] = true;
			addRecursivelyActiveFunctionNodes(activeFlags, iActive);
		}
	}
}


}