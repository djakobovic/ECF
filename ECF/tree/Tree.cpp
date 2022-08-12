#include "../ECF_base.h"
#include "Tree.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <sstream>

// operators
#include "TreeMutPermutation.h"
#include "TreeMutGauss.h"
#include "TreeMutNodeReplace.h"
#include "TreeMutNodeComplement.h"
#include "TreeMutHoist.h"
#include "TreeMutShrink.h"
#include "TreeMutSubtree.h"
#include "TreeCrxSimple.h"
#include "TreeCrxUniform.h"
#include "TreeCrxContextPreserved.h"
#include "TreeCrxSizeFair.h"
#include "TreeCrxOnePoint.h"


namespace Tree
{

Tree::Tree(void)
{
	name_ = "Tree";
	startDepth_ = 0;
}


Tree::~Tree(void)
{	}


Tree* Tree::copy()
{
	Tree *newObject = new Tree(*this);

	// create new nodes, copy primitives if necessary
	for(int i = 0; i < (int) this->size(); i++) {
		(*newObject)[i] = (NodeP) (new Node((*this)[i]));
	}
	return newObject;
}


std::vector<CrossoverOpP> Tree::getCrossoverOp()
{
	std::vector<CrossoverOpP> crx;
	crx.push_back((CrossoverOpP) (new TreeCrxSimple));
	crx.push_back((CrossoverOpP) (new TreeCrxContextPreserved));
	crx.push_back((CrossoverOpP) (new TreeCrxUniform));
	crx.push_back((CrossoverOpP) (new TreeCrxSizeFair));
	crx.push_back((CrossoverOpP) (new TreeCrxOnePoint));
	return crx;
}


std::vector<MutationOpP> Tree::getMutationOp()
{
	std::vector<MutationOpP> mut;
	mut.push_back((MutationOpP) (new TreeMutPermutation));
	mut.push_back((MutationOpP) (new TreeMutGauss));
	mut.push_back((MutationOpP) (new TreeMutNodeReplace));
	mut.push_back((MutationOpP) (new TreeMutNodeComplement));
	mut.push_back((MutationOpP) (new TreeMutHoist));
	mut.push_back((MutationOpP) (new TreeMutShrink));
	mut.push_back((MutationOpP) (new TreeMutSubtree));
	
	return mut;
}


/**
 * \brief Add user defined function primitive.
 * Must be called prior to initialization (no impact otherwise).
 */
bool Tree::addFunction(PrimitiveP func)
{
	userFunctions_.push_back(func);
	return true;
}


/**
 * \brief Add user defined terminal primitive.
 * Must be called prior to initialization (no impact otherwise).
 */
bool Tree::addTerminal(PrimitiveP term)
{
	userTerminals_.push_back(term);
	return true;
}


void Tree::registerParameters(StateP state)
{
	registerParameter(state, "maxdepth", (voidP) (new uint(5)), ECF::UINT,
		"maximum tree depth (default: 5)");
	registerParameter(state, "mindepth", (voidP) (new uint(1)), ECF::UINT,
		"minimum tree depth (default: 1)");
	registerParameter(state, "initmaxdepth", (voidP) (new uint(5)), ECF::UINT,
				"maximum initial tree depth (default: 5)");
	registerParameter(state, "initmindepth", (voidP) (new uint(1)), ECF::UINT,
				"minimum initial tree depth (default: 1)");
	registerParameter(state, "functionset", (voidP) (new std::string), ECF::STRING,
		"set of functional tree elements (mandatory)");
	registerParameter(state, "terminalset", (voidP) (new std::string), ECF::STRING,
		"set of terminal tree elements (mandatory)");
}


bool Tree::initialize(StateP state)
{
	// 'hometree' is a Tree instance kept in the State;
	// we use it to link the PrimitiveSet to it and store the parameters
	Tree* hometree = (Tree*) state->getGenotypes()[genotypeId_].get();
	state_ = state;

	// if we are the first one to initialize
	if(!hometree->primitiveSet_) 
		initializeFirst(hometree);

	// in any case, read parameters from hometree
	this->primitiveSet_ = hometree->primitiveSet_;
	initMaxDepth_ = hometree->initMaxDepth_;
	initMinDepth_ = hometree->initMinDepth_;
	maxDepth_ = hometree->maxDepth_;
	minDepth_ = hometree->minDepth_;

	// build tree (TODO: reimplement for ramped-half-and-half)
	if(state->getRandomizer()->getRandomInteger(0, 1) % 2 == 0)
		fullBuild(primitiveSet_);
	else
		growBuild(primitiveSet_);

	return true;
}


/**
 * \brief Performs the first Tree initialization (performed only once for each active Tree Genotype).
 * Constructs the primitive set and performs parameter checks.
 *
 * @param	hometree	uninitialized Tree instance in vector of active Genotypes stored in State
 */
void Tree::initializeFirst(Tree* hometree)
{
	// create and link PrimitiveSet to the hometree
	hometree->primitiveSet_ = (PrimitiveSetP) (new PrimitiveSet);
	hometree->primitiveSet_->initialize(state_);
	this->primitiveSet_ = hometree->primitiveSet_;

	// add user defined functions to primitiveSet
	for(int i = 0; i < (int) userFunctions_.size(); i++) {
		primitiveSet_->mAllPrimitives_[userFunctions_[i]->getName()] = userFunctions_[i];
		primitiveSet_->mAllPrimitives_[userFunctions_[i]->getName()]->initialize(state_);
	}

	// read function set from the configuration
	voidP sptr = getParameterValue(state_, "functionset");
	std::stringstream names;
	std::string name;
	names << *((std::string*) sptr.get());
	while(names >> name) {
		if(!primitiveSet_->addFunction(name)) {
			ECF_LOG_ERROR(state_, "Error: unknown function in function set (\'" + name + "\')!");
			throw("");
		}
	}
	if(primitiveSet_->getFunctionSetSize() == 0) {
		ECF_LOG_ERROR(state_, "Tree genotype: empty function set!");
		throw("");
	}

	// set default terminal type
	Primitives::terminal_type currentType = Primitives::Double;
	type_iter typeIter;

	// read terminal set from the configuration
	std::stringstream tNames;
	sptr = getParameterValue(state_, "terminalset");
	tNames << *((std::string*) sptr.get());

	while(tNames >> name)
	{
		// read current terminal type (if set)
		typeIter = primitiveSet_->mTypeNames_.find(name);
		if(typeIter != primitiveSet_->mTypeNames_.end()) {
			currentType = typeIter->second;
			continue;
		}

		// see if it's a user-defined terminal 
		uint iTerminal = 0;
		for(; iTerminal < userTerminals_.size(); iTerminal++)
			if(userTerminals_[iTerminal]->getName() == name)
				break;
		if(iTerminal < userTerminals_.size()) {
			userTerminals_[iTerminal]->initialize(state_);
			primitiveSet_->addTerminal(userTerminals_[iTerminal]);
			continue;
		}


		// read ERC definition (if set)
		// ERC's are defined as interval [x y] or set {a b c}
		if(name[0] == '[' || name[0] == '{') {

			std::string ercValues = "";

			// name this ERC (ERC's name is its value!)
			PrimitiveP erc;
			switch(currentType) {
				case Primitives::Double:
					erc = (PrimitiveP) (new Primitives::ERCD); 
					ercValues = DBL_PREFIX;
					break;
				case Primitives::Int:
					erc = (PrimitiveP) (new Primitives::ERC<int>); 
					ercValues = INT_PREFIX;
					break;
				case Primitives::Bool:
					erc = (PrimitiveP) (new Primitives::ERC<bool>); 
					ercValues = BOOL_PREFIX;
					break;
				case Primitives::Char:
					erc = (PrimitiveP) (new Primitives::ERC<char>); 
					ercValues = CHR_PREFIX;
					break;
				case Primitives::String:
					erc = (PrimitiveP) (new Primitives::ERC<std::string>); 
					ercValues = STR_PREFIX;
					break;
			}

			while(name[name.size() - 1] != ']' && name[name.size() - 1] != '}') {
				ercValues += " " + name;
				tNames >> name;
			}
			ercValues += " " + name;

			// ERC ocekuje "D_ [<value> <value> ...]" kao ime prije initialize()
			erc->setName(ercValues);
			erc->initialize(state_);
			primitiveSet_->addTerminal(erc);

			continue;
		} 


		// otherwise, read terminal of current type
		PrimitiveP terminal;
		switch(currentType)
		{
			case Primitives::Double:
				terminal = (PrimitiveP) (new Primitives::Terminal); break;
			case Primitives::Int:
				terminal = (PrimitiveP) (new Primitives::TerminalT<int>); break;
			case Primitives::Bool:
				terminal = (PrimitiveP) (new Primitives::TerminalT<bool>); break;
			case Primitives::Char:
				terminal = (PrimitiveP) (new Primitives::TerminalT<char>); break;
			case Primitives::String:
				terminal = (PrimitiveP) (new Primitives::TerminalT<std::string>); break;
		}

		// if the 'name' can be identified as a value of the 'currentType', then it's a _constant terminal_ (of that value)
		// otherwise, it's a regular terminal with 'name'
		std::istringstream ss(name);
		switch(currentType)
		{
			case Primitives::Double:
				double dblValue;
				ss >> dblValue;
				if(ss.fail() == false) 
					terminal->setValue(&dblValue);
				break;
			case Primitives::Int:
				int intValue;
				ss >> intValue;
				if(ss.fail() == false) 
					terminal->setValue(&intValue);
				break;
			case Primitives::Bool:
				bool boolValue;
				ss >> boolValue;
				if(name == "true")
					boolValue = true;
				else if(name == "false")
					boolValue = false;
				if(ss.fail() == false || name == "true" || name == "false") {
					if(boolValue)
						name = "true";
					else
						name = "false";
					terminal->setValue(&boolValue);
				}
				break;
			case Primitives::Char:
				char charValue;
				ss >> charValue;
				if(ss.fail() == false) 
					terminal->setValue(&charValue);
				break;
			case Primitives::String:
				std::string stringValue;
				ss >> stringValue;
				if(ss.fail() == false) 
					terminal->setValue(&stringValue);
				break;
		}
		terminal->setName(name);
		primitiveSet_->addTerminal(terminal);
		
	}

	if(primitiveSet_->getTerminalSetSize() == 0) {
		ECF_LOG_ERROR(state_, "Tree: Empty terminal set!");
		throw("");
	}

	// read tree depth constraints, store in hometree
	sptr = getParameterValue(state_, "maxdepth");
	hometree->maxDepth_ = *((uint*) sptr.get());
	sptr = getParameterValue(state_, "mindepth");
	hometree->minDepth_ = *((uint*) sptr.get());
	if(hometree->maxDepth_ < hometree->minDepth_ || hometree->maxDepth_ < 1) {
		ECF_LOG_ERROR(state_, "Tree genotype: invalid values for max and min tree depth!");
		throw("");
	}

	hometree->initMaxDepth_ = hometree->maxDepth_;
	if(isParameterDefined(state_, "initmaxdepth")) {
		sptr = getParameterValue(state_, "initmaxdepth");
		hometree->initMaxDepth_ = *((uint*) sptr.get());
	}
	hometree->initMinDepth_ = hometree->minDepth_;
	if(isParameterDefined(state_, "initmindepth")) {
		sptr = getParameterValue(state_, "initmindepth");
		hometree->initMinDepth_ = *((uint*) sptr.get());
	}
	if(hometree->initMaxDepth_ < hometree->initMinDepth_ || hometree->initMaxDepth_ < 1) {
		ECF_LOG_ERROR(state_, "Tree genotype: invalid values for initial max and min tree depth!");
		throw("");
	}
	if(hometree->initMaxDepth_ > hometree->maxDepth_) {
		ECF_LOG_ERROR(state_, "Tree genotype: initial max depth cannot be greater than evolution max depth!");
		throw("");
	}
}


/**
 * \brief Execute current tree.
 *
 * Evaluates the function embodied in the tree (or executes the represented program).
 * \param result	the resulting tree value (of the first node)
 */
void Tree::execute(void *result)
{
	iNode_ = 0;
	this->at(iNode_)->primitive_->execute(result, *this);
}


void Tree::addNode(Node *node)
{
	this->push_back(static_cast<NodeP> (node));
}


void Tree::addNode(NodeP node)
{
	this->push_back(node);
}


/// Build Tree using 'full' method
uint Tree::fullBuild(PrimitiveSetP primitiveSet, uint myDepth)
{
	Node* node = new Node();
	node->depth_ = myDepth;

	if(node->depth_ < this->initMaxDepth_) {
		node->setPrimitive(primitiveSet->getRandomFunction());
		this->addNode(node);
	} else {
		node->setPrimitive(primitiveSet->getRandomTerminal());
		this->addNode(node);
	}

	for(int i = 0; i < node->primitive_->getNumberOfArguments(); i++ ) {
		node->size_ += fullBuild(primitiveSet, myDepth + 1);
	}

	return node->size_;
}


/// Build Tree using 'grow' method
uint Tree::growBuild(PrimitiveSetP primitiveSet, uint myDepth)
{
	Node* node = new Node();
	node->depth_ = myDepth;

	if(node->depth_ < this->initMinDepth_) {
		node->setPrimitive(primitiveSet->getRandomFunction());
		this->addNode(node);
	}
	else if(node->depth_ < this->initMaxDepth_) {
		node->setPrimitive(primitiveSet->getRandomPrimitive());
		this->addNode(node);
	}
	else {
		node->setPrimitive(primitiveSet->getRandomTerminal());
		this->addNode(node);
	}
	
	for(int i = 0; i < node->primitive_->getNumberOfArguments(); i++) {
		node->size_ += growBuild(primitiveSet, myDepth + 1);
	}

	return node->size_;
}


/**
 * \brief Calculate depth and subtree sizes of each node in the tree.
 *
 * Uses recursive depth-first tree processing.
 */
void Tree::update()
{
	this->at(0)->size_ = setSize(0);

	iNode_ = 0;
	for(int i = 0; i < this->at(0)->primitive_->getNumberOfArguments(); i++) {
		iNode_++;
		setDepth(startDepth_ + 1);
	}
	this->at(0)->depth_ = startDepth_;
}


/**
 * \brief Calculate subtree sizes of each node in the tree.
 */
uint Tree::setSize(uint iNode)
{
	uint myNode = iNode;
	uint mySize = 1;
	for(int i = 0; i < this->at(myNode)->primitive_->getNumberOfArguments(); i++) {
		uint childSize = setSize(iNode + 1);
		mySize += childSize;
		iNode += childSize;
	}
	this->at(myNode)->size_ = mySize;
	return mySize;
}


/**
 * \brief Calculate depth of each node in the tree.
 */
void Tree::setDepth(uint myDepth)
{
	uint index = iNode_;
	int nArgs = this->at( iNode_ )->primitive_->getNumberOfArguments();
	for(int i = 0; i < nArgs; i++) {
		iNode_++;
		setDepth(myDepth + 1);
	}
	this->at(index)->depth_ = myDepth;
}


/**
 * \brief Build tree with grow method.
 */
void Tree::growBuild(PrimitiveSetP primitiveSet)
{
	growBuild(primitiveSet, startDepth_);
}


/**
 * \brief Build tree with full method.
 */
void Tree::fullBuild(PrimitiveSetP primitiveSet)
{
	fullBuild(primitiveSet, startDepth_);
}


/**
 * \brief Set a terminal's value.
 * 
 * \param name	terminal's name
 * \param value	terminal's value
 */
void Tree::setTerminalValue(std::string name, void* value)
{
	PrimitiveP term = primitiveSet_->getTerminalByName(name);
	if(term == PrimitiveP()) {
		ECF_LOG_ERROR(state_, "Tree genotype: invalid terminal name referenced in setTerminalValue()!");
		throw("");
	}

	term->setValue(value);
}


/**
 * \brief Retrieve a terminal's value.
 * 
 * \param name	terminal's name
 * \param value	terminal's value
 */
void Tree::getTerminalValue(std::string name, void* value)
{
	PrimitiveP term = primitiveSet_->getTerminalByName(name);
	if(term == PrimitiveP()) {
		ECF_LOG_ERROR(state_, "Tree genotype: invalid terminal name referenced in getTerminalValue()!");
		throw("");
	}

	term->getValue(value);
}



void Tree::write(XMLNode& xTree)
{
	xTree = XMLNode::createXMLTopNode("Tree");
	std::stringstream sValue;
	sValue << this->size();
	xTree.addAttribute("size", sValue.str().c_str());
	
	sValue.str("");
	for(uint i = 0; i < this->size(); i++) {
		sValue << this->at(i)->primitive_->getName() << " ";
	}
	xTree.addText(sValue.str().c_str());
}


void Tree::read(XMLNode& xTree)
{
	this->clear();

	XMLCSTR sizeStr = xTree.getAttribute("size");
	uint size = str2uint(sizeStr);

	XMLCSTR tree = xTree.getText();
	std::stringstream stream;
	stream << tree;

	std::vector<PrimitiveP>& primitives = primitiveSet_->primitives_;
	std::string primitiveStr;
	uint position = 0;

	for(uint iNode = 0; iNode < size; iNode++) {
		stream >> primitiveStr;
		Node* node = new Node();

		// 'regular' primitives
		PrimitiveP prim = primitiveSet_->getPrimitiveByName(primitiveStr);
		if(prim != PrimitiveP()) {
			node->setPrimitive(prim);
			this->addNode(node);
			continue;
		}

		// ERCs
		// (TODO: include user defined ERC types)
		PrimitiveP erc;
		std::string prefix = primitiveStr.substr(0, 2);
		std::string value = primitiveStr.substr(2);
		std::stringstream ss;
		ss << value;
		if(prefix == DBL_PREFIX) {
			erc = (PrimitiveP) (new Primitives::ERCD);
			double v;
			ss >> v;
			erc->setValue(&v);
		}
		else if(prefix == INT_PREFIX) {
			erc = (PrimitiveP) (new Primitives::ERC<int>);
			int v;
			ss >> v;
			erc->setValue(&v);
		}
		else if(prefix == BOOL_PREFIX) {
			erc = (PrimitiveP) (new Primitives::ERC<bool>);
			bool v;
			ss >> v;
			erc->setValue(&v);
		}
		else if(prefix == CHR_PREFIX) {
			erc = (PrimitiveP) (new Primitives::ERC<char>);
			char v;
			ss >> v;
			erc->setValue(&v);
		}
		else if(prefix == STR_PREFIX) {
			erc = (PrimitiveP) (new Primitives::ERC<std::string>);
			std::string v;
			ss >> v;
			erc->setValue(&v);
		}
		else {
			ECF_LOG_ERROR(state_, "Tree genotype: undefined primitive (" + primitiveStr + ")!");
			throw("");
		}
		erc->setName(primitiveStr);
		node->primitive_ = erc;
		this->addNode(node);
	}

	// set node depths and sizes
	this->update();
}

}