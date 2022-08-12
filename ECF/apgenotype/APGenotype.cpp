#include <iostream>
#include <cmath>
#include "APGenotype.h"


namespace Tree {


void APGenotype::registerParameters(StateP state)
{
	registerParameter(state, "lbound", (voidP) new double(-10), ECF::DOUBLE,
		"lower bound for each variable (mandatory)");
	registerParameter(state, "ubound", (voidP) new double(10), ECF::DOUBLE,
		"upper bound for each variable (mandatory)");
	registerParameter(state, "dimension", (voidP) new uint(1), ECF::UINT,
		"number of real valued variables (mandatory)");

	registerParameter(state, "functionset", (voidP)(new std::string), ECF::STRING,
		"set of functional tree elements (mandatory)");
	registerParameter(state, "terminalset", (voidP)(new std::string), ECF::STRING,
		"set of terminal tree elements (mandatory)");
}


bool APGenotype::addFunction(PrimitiveP func)
{
	userFunctions_.push_back(func);
	return true;
}


bool APGenotype::addTerminal(PrimitiveP term)
{
	userTerminals_.push_back(term);
	return true;
}


bool APGenotype::initialize(StateP state)
{
	state_ = state;

	// 'persistent' is the AP genotype instance kept in the State;
	// we use it to link the PrimitiveSet to it and store the parameters
	APGenotype* persistent = (APGenotype*)state->getGenotypes()[genotypeId_].get();

	// if we are the first one to initialize
	if (!persistent->primitiveSet)
		initializeFirst(persistent);

	// in any case, read parameters from hometree
	this->primitiveSet = persistent->primitiveSet;
	this->maxValue_ = persistent->maxValue_;
	this->minValue_ = persistent->minValue_;
	this->nDimension_ = persistent->nDimension_;

	// create this genotype instance
	realValue.resize(nDimension_);
	// randomly create each dimension
	for (uint i = 0; i < nDimension_; i++) {
		realValue[i] = (minValue_ + (maxValue_ - minValue_) * state->getRandomizer()->getRandomDouble());
	}

	return true;
}


void APGenotype::initializeFirst(APGenotype* persistent)
{
	// create and link PrimitiveSet to the persistent object
	persistent->primitiveSet = (PrimitiveSetAPP)(new PrimitiveSetAP);
	persistent->primitiveSet->initialize(state_);
	this->primitiveSet = persistent->primitiveSet;

	if (!isParameterDefined(state_, "lbound") ||
		!isParameterDefined(state_, "ubound") ||
		!isParameterDefined(state_, "dimension")) {
		ECF_LOG_ERROR(state_, "Error: required parameters not defined (lbound, ubound, dimension)!");
		throw("");
	}

	voidP genp = getParameterValue(state_, "lbound");
	persistent->minValue_ = *((double*)genp.get());

	genp = getParameterValue(state_, "ubound");
	persistent->maxValue_ = *((double*)genp.get());

	if (persistent->minValue_ >= persistent->maxValue_) {
		ECF_LOG_ERROR(state_, "Error: 'lbound' must be smaller than 'ubound'!");
		throw("");
	}

	genp = getParameterValue(state_, "dimension");
	persistent->nDimension_ = *((uint*)genp.get());

	if (persistent->nDimension_ < 1) {
		ECF_LOG_ERROR(state_, "Error: 'dimension' must be > 0 !");
		throw("");
	}

	// add user defined functions to primitiveSet
	for (int i = 0; i < (int)userFunctions_.size(); i++) {
		primitiveSet->mAllPrimitives_[userFunctions_[i]->getName()] = userFunctions_[i];
		primitiveSet->mAllPrimitives_[userFunctions_[i]->getName()]->initialize(state_);
	}

	voidP sptr = getParameterValue(state_, "functionset");
	std::stringstream names;
	std::string name;
	names << *((std::string*) sptr.get());
	while (names >> name) {
		if (!primitiveSet->addFunction(name)) {
			ECF_LOG_ERROR(state_, "Error: unknown function in function set (\'" + name + "\')!");
			throw("");
		}

	}

	if (primitiveSet->getFunctionSetSize() == 0) {
		ECF_LOG_ERROR(state_, "AP genotype: empty function set!");
		throw("");
	}

	// set default terminal type
	Primitives::terminal_type currentType = Primitives::Double;
	type_iter typeIter;

	// read terminal set from the configuration
	std::stringstream tNames;
	sptr = getParameterValue(state_, "terminalset");
	tNames << *((std::string*) sptr.get());

	while (tNames >> name)
	{
		// read current terminal type (if set)
		typeIter = primitiveSet->mTypeNames_.find(name);
		if (typeIter != primitiveSet->mTypeNames_.end()) {
			currentType = typeIter->second;
			continue;
		}

		// see if it's a user-defined terminal 
		uint iTerminal = 0;
		for (; iTerminal < userTerminals_.size(); iTerminal++)
			if (userTerminals_[iTerminal]->getName() == name)
				break;
		if (iTerminal < userTerminals_.size()) {
			userTerminals_[iTerminal]->initialize(state_);
			primitiveSet->addTerminal(userTerminals_[iTerminal]);
			continue;
		}


		// read ERC definition (if set)
		// ERC's are defined as interval [x y] or set {a b c}
		if (name[0] == '[' || name[0] == '{') {

			std::string ercValues = "";

			// name this ERC (ERC's name is its value!)
			PrimitiveP erc;
			switch (currentType) {
			case Primitives::Double:
				erc = (PrimitiveP)(new Primitives::ERCD);
				ercValues = DBL_PREFIX;
				break;
			case Primitives::Int:
				erc = (PrimitiveP)(new Primitives::ERC<int>);
				ercValues = INT_PREFIX;
				break;
			case Primitives::Bool:
				erc = (PrimitiveP)(new Primitives::ERC<bool>);
				ercValues = BOOL_PREFIX;
				break;
			case Primitives::Char:
				erc = (PrimitiveP)(new Primitives::ERC<char>);
				ercValues = CHR_PREFIX;
				break;
			case Primitives::String:
				erc = (PrimitiveP)(new Primitives::ERC<std::string>);
				ercValues = STR_PREFIX;
				break;
			}

			while (name[name.size() - 1] != ']' && name[name.size() - 1] != '}') {
				ercValues += " " + name;
				tNames >> name;
			}
			ercValues += " " + name;

			// ERC ocekuje "D_ [<value> <value> ...]" kao ime prije initialize()
			erc->setName(ercValues);
			erc->initialize(state_);
			primitiveSet->addTerminal(erc);

			continue;
		}


		// otherwise, read terminal of current type
		PrimitiveP terminal;
		switch (currentType)
		{
		case Primitives::Double:
			terminal = (PrimitiveP)(new Primitives::Terminal); break;
		case Primitives::Int:
			terminal = (PrimitiveP)(new Primitives::TerminalT<int>); break;
		case Primitives::Bool:
			terminal = (PrimitiveP)(new Primitives::TerminalT<bool>); break;
		case Primitives::Char:
			terminal = (PrimitiveP)(new Primitives::TerminalT<char>); break;
		case Primitives::String:
			terminal = (PrimitiveP)(new Primitives::TerminalT<std::string>); break;
		}

		// if the 'name' can be identified as a value of the 'currentType', then it's a _constant terminal_ (of that value)
		// otherwise, it's a regular terminal with 'name'
		std::istringstream ss(name);
		switch (currentType)
		{
		case Primitives::Double:
			double dblValue;
			ss >> dblValue;
			if (ss.fail() == false)
				terminal->setValue(&dblValue);
			break;
		case Primitives::Int:
			int intValue;
			ss >> intValue;
			if (ss.fail() == false)
				terminal->setValue(&intValue);
			break;
		case Primitives::Bool:
			bool boolValue;
			ss >> boolValue;
			if (name == "true")
				boolValue = true;
			else if (name == "false")
				boolValue = false;
			if (ss.fail() == false || name == "true" || name == "false") {
				if (boolValue)
					name = "true";
				else
					name = "false";
				terminal->setValue(&boolValue);
			}
			break;
		case Primitives::Char:
			char charValue;
			ss >> charValue;
			if (ss.fail() == false)
				terminal->setValue(&charValue);
			break;
		case Primitives::String:
			std::string stringValue;
			ss >> stringValue;
			if (ss.fail() == false)
				terminal->setValue(&stringValue);
			break;
		}
		terminal->setName(name);
		primitiveSet->addTerminal(terminal);

	}

	if (primitiveSet->getTerminalSetSize() == 0) {
		ECF_LOG_ERROR(state_, "APGenotype: Empty terminal set!");
		throw("");
	}

}


std::vector<uint> APGenotype::getDiscreteIndices() 
{
	std::vector<uint> discrete;
	discrete.resize(nDimension_);

	double dindex, diff, delta;
	int  imax = primitiveSet->getPrimitivesSize() - 1, dis_index;
	diff = maxValue_ - minValue_;
	delta = 1.0 / (double)primitiveSet->getPrimitivesSize();

	for (uint i = 0; i < nDimension_; i++) {
		dis_index = 0;
		dindex = (realValue[i] - minValue_) / diff;

		dindex -= delta;
		while (dindex > 0 && dis_index != imax) {
			dindex -= delta;
			dis_index++;
		}

		discrete[i] = dis_index;
	}

	return discrete;
}


void APGenotype::buildTree(std::vector<uint> indices, uint current, uint depth)
{
	Node* node = new Node();
	node->depth_ = depth;

	uint index = current - 1;

	uint numArgs = primitiveSet->getPrimitiveByIndex(index)->getNumberOfArguments();

	if (numArgs < toEnd) {
		node->setPrimitive(primitiveSet->getPrimitiveByIndex(index));
		nodes.push_back(static_cast<NodeP> (node));
	} else if (toEnd >= 2 && toEnd <= numArgs && primitiveSet->getGFSTwoSetSize() != 0) {
		node->setPrimitive(primitiveSet->getGFSTwoByIndex(index));
		nodes.push_back(static_cast<NodeP> (node));
	} else if (toEnd == 1 && primitiveSet->getGFSOneSetSize() != 0) {
		node->setPrimitive(primitiveSet->getGFSOneByIndex(index));
		nodes.push_back(static_cast<NodeP> (node));
	} else  {
		node->setPrimitive(primitiveSet->getTerminalByIndex(index));
		nodes.push_back(static_cast<NodeP> (node));
	}

	numArgs = node->primitive_->getNumberOfArguments();

	toEnd -= numArgs;

	if (numArgs > 0) {
		std::vector<uint> passableIndices(indices.begin(), indices.begin() + numArgs);
		indices.erase(indices.begin(), indices.begin() + numArgs - 1);

		for (uint i = 0; i < numArgs; i++) {
			buildTree(indices, passableIndices[i], depth + 1);
		}
	}
}


Tree* APGenotype::convertToPhenotype() 
{
	std::vector<uint> indices = getDiscreteIndices();

	toEnd = nDimension_ - 1;
	uint first = indices[0];
	nodes.clear();

	indices.erase(indices.begin());

	buildTree(indices, first, 0);

	Tree* tree = new Tree();
	for (uint i = 0; i < nodes.size(); i++) {
		tree->addNode(nodes[i]);
	}

	std::stringstream sValue;
	for (uint i = 0; i < tree->size(); i++) {
		sValue << (*tree)[i]->primitive_->getName() << " ";
	}

	//cout << sValue.str() << endl;

	return tree;
}


void APGenotype::setTerminalValue(Tree* tree, std::string name, void* value)
{
	PrimitiveP term = primitiveSet->getTerminalByName(name);
	if (term == PrimitiveP()) {
		ECF_LOG_ERROR(state_, "AP genotype: invalid terminal name referenced in setTerminalValue()!");
		throw("");
	}

	term->setValue(value);
}


void APGenotype::write(XMLNode &xFloatingPoint)
{
	xFloatingPoint = XMLNode::createXMLTopNode("APGenotype");
	std::stringstream sValue;
	sValue << nDimension_;
	xFloatingPoint.addAttribute("size", sValue.str().c_str());

	sValue.str("");
	for (uint iVar = 0; iVar < nDimension_; iVar++)
		sValue << "\t" << realValue[iVar];
	xFloatingPoint.addText(sValue.str().c_str());
}


void APGenotype::read(XMLNode& xFloatingPoint)
{
	XMLCSTR values = xFloatingPoint.getText();
	std::stringstream sValues;
	sValues << values;

	for (uint iVar = 0; iVar < nDimension_; iVar++) {
		sValues >> realValue[iVar];
	}
}


}