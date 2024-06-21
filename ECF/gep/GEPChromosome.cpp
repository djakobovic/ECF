#include "GEPChromosome.h"

namespace GEP{
	// mandatory: define name, other things as needed
	GEPChromosome::GEPChromosome(){
		Genotype::name_ = "GEPChromosome";
		usesERC = false;
		dcLength = 0;
		staticLink = false;
	}

	// mandatory: must provide copy method
	GEPChromosome* GEPChromosome::copy()
	{
		GEPChromosome *newObject = new GEPChromosome(*this);
		return newObject;
	}

	// optional: declare crx operators (if not, no crossover will be performed)
	std::vector<CrossoverOpP> GEPChromosome::getCrossoverOp()
	{
		std::vector<CrossoverOpP> crx;
		crx.push_back(static_cast<CrossoverOpP> (new GEPChromosomeCrsOnePoint));
		crx.push_back(static_cast<CrossoverOpP> (new GEPChromosomeCrsTwoPoint));
		crx.push_back(static_cast<CrossoverOpP> (new GEPChromosomeCrsGene));
		return crx;
	}

	// optional: declare mut operators (if not, no mutation will be performed)
	std::vector<MutationOpP> GEPChromosome::getMutationOp()
	{
		std::vector<MutationOpP> mut;
		mut.push_back(static_cast<MutationOpP> (new GEPChromosomeMutOp));
		mut.push_back(static_cast<MutationOpP> (new GEPChromosomeMutGauss));
		return mut;
	}

	// optional: register any parameters
	void GEPChromosome::registerParameters(StateP state)
	{
		registerParameter(state, "headlength", (voidP)(new uint(1)), ECF::UINT);
		registerParameter(state, "genes", (voidP)(new uint(1)), ECF::UINT);
		registerParameter(state, "functionset", (voidP)(new std::string), ECF::STRING);
		registerParameter(state, "terminalset", (voidP)(new std::string), ECF::STRING);
		registerParameter(state, "linkingfunctions", (voidP)(new std::string), ECF::STRING);
		registerParameter(state, "linklength", (voidP)(new uint(1)), ECF::UINT);
	}


	/**
	 * \brief Add user defined function primitive.
	 * Must be called prior to initialization (no impact otherwise).
	 */
	bool GEPChromosome::addFunction(Tree::PrimitiveP func)
	{
		userFunctions_.push_back(func);
		return true;
	}


	void GEPChromosome::generateChromosome()
	{
		Tree::Node* node;
		for (uint i = 0; i < genes; i++){
			// Generate random primitives for the head (Functions + Terminals)
			for (uint j = 0; j < headLength; j++) {
				node = new Tree::Node();
				node->setPrimitive(primitiveSet_->getRandomPrimitive());
				this->push_back(static_cast<Tree::NodeP>(node)); 
			}
			// Generate random terminals for the tail
			for (uint j = 0; j < tailLength; j++) {
				node = new Tree::Node();
				node->setPrimitive(primitiveSet_->getRandomTerminal());
				this->push_back(static_cast<Tree::NodeP>(node));
			}
			// Generate ERCs for the Dc domain
			for (uint j = 0; j < dcLength; j++){
				node = new Tree::Node();
				node->setPrimitive(ercSet_->getRandomTerminal());
				this->push_back(static_cast<Tree::NodeP>(node));
			}
		}
		// Set the homeotic gene (this controls the linking functions)
		for (uint i = 0; i < linkHeadLength; i++){
			node = new Tree::Node();
			node->setPrimitive(linkFunctionSet_->getRandomPrimitive());
			this->push_back(static_cast<Tree::NodeP>(node));
		}
		for (uint i = 0; i < linkTailLength; i++){
			node = new Tree::Node();
			node->setPrimitive(linkFunctionSet_->getRandomTerminal());
			this->push_back(static_cast<Tree::NodeP>(node));
		}
	}

	// mandatory: build initial genotype structure
	bool GEPChromosome::initialize(StateP state)
	{
		// 'homegep' is a Gep instance kept in the State;
		// we use it to link the PrimitiveSet to it and store the parameters
		GEPChromosome* homegep = (GEPChromosome*)state->getGenotypes()[genotypeId_].get();
		state_ = state;

		// if we are the first one to initialize
		if (!homegep->primitiveSet_){
			initializeFirst(homegep);
		}
		// in any case, read parameters from from hometree
		this->primitiveSet_ = homegep->primitiveSet_;
		this->linkFunctionSet_ = homegep->linkFunctionSet_;
		this->ercSet_ = homegep->ercSet_;
		this->headLength = homegep->headLength;
		this->genes = homegep->genes;
		this->tailLength = homegep->tailLength;
		this->dcLength = homegep->dcLength;
		this->geneLength = homegep->geneLength;
		this->linkHeadLength = homegep->linkHeadLength;
		this->linkTailLength = homegep->linkTailLength;
		// generate the chromosome
		generateChromosome();

		return true;
	}

	void GEPChromosome::initializeFirst(GEPChromosome* home)
	{

		// create and link PrimitiveSet to the hometree
		if (home == NULL){
			return;
		}
		home->primitiveSet_ = static_cast<Tree::PrimitiveSetP> (new Tree::PrimitiveSet);
		home->primitiveSet_->initialize(state_);
		this->primitiveSet_ = home->primitiveSet_;

		home->linkFunctionSet_ = static_cast<Tree::PrimitiveSetP> (new Tree::PrimitiveSet);
		home->linkFunctionSet_->initialize(state_);
		this->linkFunctionSet_ = home->linkFunctionSet_;

		home->ercSet_ = static_cast<Tree::PrimitiveSetP> (new Tree::PrimitiveSet);
		home->ercSet_->initialize(state_);
		this->ercSet_ = home->ercSet_;

		// read number of genes, store in hometree
		voidP sptr = getParameterValue(state_, "genes");
		home->genes = *((uint*)sptr.get());

		if (home->genes < 1) {
			ECF_LOG_ERROR(state_, "Gep genotype: number of genes must be >=1");
		}

		// add user defined functions to primitiveSet
		
		for (int i = 0; i < (int)userFunctions_.size(); i++) {
		primitiveSet_->mAllPrimitives_[userFunctions_[i]->getName()] = userFunctions_[i];
		}
		
		uint maxArg = 0;
		uint tmpArg = 0;
		// read function set from the configuration
		sptr = getParameterValue(state_, "functionset");
		std::stringstream names;
		std::string name;
		names << *((std::string*) sptr.get());
		while (names >> name) {
			if (!primitiveSet_->addFunction(name)) {
				ECF_LOG_ERROR(state_, "Error: unknown function in function set (\'" + name + "\')!");
				throw("");
			}
			tmpArg = primitiveSet_->getPrimitiveByName(name)->getNumberOfArguments();
			if (tmpArg > maxArg)
				maxArg = tmpArg;
		}
		// read Gep head length, store in hometree
		sptr = getParameterValue(state_, "headlength");
		home->headLength = *((uint*)sptr.get());

		if (home->headLength < 1) {
			ECF_LOG_ERROR(state_, "Gep genotype: length of head must be >= 1");
		}

		// now we can tell how long tail must be
		home->tailLength = home->headLength * (maxArg - 1) + 1;
		home->geneLength = home->headLength + home->tailLength;

		if (primitiveSet_->getFunctionSetSize() == 0) {
			ECF_LOG_ERROR(state_, "Tree genotype: empty function set!");
			throw("");
		}

		// create and link the linking function set
		// Mono-genic chromosomes should have a constant homeotic gene for the sake of simplicity (i.e., their homeotic gene shall always be "0")
		// Multi-genic chromosomes can evolve their homeotic gene by default, unless a static linking configuration is specified (TO-DO)

		uint linkMaxArg = 0;
		uint linkTmpArg = 0;
		std::stringstream linkNames;
		// read linking function set from the configuration
		sptr = getParameterValue(state_, "linkingfunctions");
		linkNames << *((std::string*) sptr.get());
		while (linkNames >> name) {
			if (!linkFunctionSet_->addFunction(name)) {
				ECF_LOG_ERROR(state_, "Error: unknown function in linking function set (\'" + name + "\')!");
				throw("");
			}
			linkTmpArg = linkFunctionSet_->getPrimitiveByName(name)->getNumberOfArguments();
			if (linkTmpArg > linkMaxArg)
				linkMaxArg = linkTmpArg;
		}
		// read homeotic gene head length, store in hometree
		sptr = getParameterValue(state_, "linklength");
		home->linkHeadLength = *((uint*)sptr.get());

		if (home->linkHeadLength < 1) {
			ECF_LOG_ERROR(state_, "Gep genotype: length of linking function gene head must be >= 1");
		}

		// now we can tell how long tail must be
		home->linkTailLength = home->linkHeadLength * (linkMaxArg - 1) + 1;

		if (linkFunctionSet_->getFunctionSetSize() == 0) {
			ECF_LOG_ERROR(state_, "GEP genotype: empty linking function set!");
		}
		// Add "terminals" to the linking function set. These will be a prefix + integers from [0, # of genes]
		for (uint i = 0; i < home->genes; i++){
			Tree::PrimitiveP geneTerminals = (Tree::PrimitiveP)(new Tree::Primitives::Terminal);
			std::string geneTermStr = GEP_GENE_PREFIX;
			geneTermStr += uint2str(i);
			geneTerminals->setName(geneTermStr);
			geneTerminals->initialize(state_);
			linkFunctionSet_->addTerminal(geneTerminals);
		}
		// set default terminal type
		Tree::Primitives::terminal_type currentType = Tree::Primitives::Double;
		Tree::type_iter typeIter;

		// read terminal set from the configuration

		std::stringstream tNames;
		sptr = getParameterValue(state_, "terminalset");
		tNames << *((std::string*) sptr.get());

		while (tNames >> name) {
			// read current terminal type (if set)
			typeIter = primitiveSet_->mTypeNames_.find(name);
			if (typeIter != primitiveSet_->mTypeNames_.end()) {
				currentType = typeIter->second;
				continue;
			}
			
			// see if it's a user-defined terminal
			/*
			uint iTerminal = 0;
			for (; iTerminal < userTerminals_.size(); iTerminal++)
			if (userTerminals_[iTerminal]->getName() == name)
			break;
			if (iTerminal < userTerminals_.size()) {
			primitiveSet_->addTerminal(userTerminals_[iTerminal]);
			continue;
			}
			*/
			// read ERC definition (if set)
			// ERC's are defined as interval [x y] or set {a b c}
			// If ERCs are requested by the user, we add the placeholder terminal '?' to primitiveSet_
			// We then add any ERCs to ercSet_ 

			if (name[0] == '[' || name[0] == '{') {

				//if this is the first ERC range we encounter, add the placeholder and switch on the ERC flag
				if (!usesERC){
					usesERC = true;
					Tree::PrimitiveP placeholder = (Tree::PrimitiveP) (new Tree::Primitives::Terminal);
					placeholder->setName("?");
					primitiveSet_->addTerminal(placeholder);
					// If ERCs are used, the length of the Dc domain is the same as the tail length
					home->dcLength = home->tailLength;
					home->geneLength += home->dcLength;
				}

				std::string ercValues = "";
				
				// name this ERC (ERC's name is its value!)
				Tree::PrimitiveP erc;
				switch (currentType) {
				case Tree::Primitives::Double:
					erc = (Tree::PrimitiveP)(new Tree::Primitives::ERCD);
					ercValues = DBL_PREFIX;
					break;
				case Tree::Primitives::Int:
					erc = (Tree::PrimitiveP)(new Tree::Primitives::ERC<int>);
					ercValues = INT_PREFIX;
					break;
				case Tree::Primitives::Bool:
					erc = (Tree::PrimitiveP)(new Tree::Primitives::ERC<bool>);
					ercValues = BOOL_PREFIX;
					break;
				case Tree::Primitives::Char:
					erc = (Tree::PrimitiveP)(new Tree::Primitives::ERC<char>);
					ercValues = CHR_PREFIX;
					break;
				case Tree::Primitives::String:
					erc = (Tree::PrimitiveP)(new Tree::Primitives::ERC<std::string>);
					ercValues = STR_PREFIX;
					break;
				}

				while (name[name.size() - 1] != ']' && name[name.size() - 1] != '}') {
					ercValues += " " + name;
					tNames >> name;
				}
				ercValues += " " + name;
				erc->setName(ercValues);
				erc->initialize(state_);
				ercSet_->addTerminal(erc);
				
				continue;
			}

			//read terminal of current type
			Tree::PrimitiveP terminal;
			switch (currentType)
			{
			case Tree::Primitives::Double:
				terminal = (Tree::PrimitiveP) (new Tree::Primitives::Terminal); break;
			case Tree::Primitives::Int:
				terminal = (Tree::PrimitiveP) (new Tree::Primitives::TerminalT<int>); break;
			case Tree::Primitives::Bool:
				terminal = (Tree::PrimitiveP) (new Tree::Primitives::TerminalT<bool>); break;
			case Tree::Primitives::Char:
				terminal = (Tree::PrimitiveP) (new Tree::Primitives::TerminalT<char>); break;
			case Tree::Primitives::String:
				terminal = (Tree::PrimitiveP) (new Tree::Primitives::TerminalT<std::string>); break;
			}

			// if the 'name' can be identified as a value of the 'currentType', then it's a _constant terminal_ (of that value)
			// otherwise, it's a regular terminal with 'name'
			std::istringstream ss(name);
			switch (currentType)
			{
			case Tree::Primitives::Double:
				double dblValue;
				ss >> dblValue;
				if (ss.fail() == false)
					terminal->setValue(&dblValue);
				break;
			case Tree::Primitives::Int:
				int intValue;
				ss >> intValue;
				if (ss.fail() == false)
					terminal->setValue(&intValue);
				break;
			case Tree::Primitives::Bool:
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
			case Tree::Primitives::Char:
				char charValue;
				ss >> charValue;
				if (ss.fail() == false)
					terminal->setValue(&charValue);
				break;
			case Tree::Primitives::String:
				std::string stringValue;
				ss >> stringValue;
				if (ss.fail() == false)
					terminal->setValue(&stringValue);
				break;
			}
			terminal->setName(name);
			primitiveSet_->addTerminal(terminal);

		}

		if (primitiveSet_->getTerminalSetSize() == 0) {
			ECF_LOG_ERROR(state_, "Tree: Empty terminal set!");
			throw("");
		}

	}

	// mandatory: write to XMLNode
	void GEPChromosome::write(XMLNode &xGEPChromosome)
	{
		xGEPChromosome = XMLNode::createXMLTopNode("GEPChromosome");
		std::stringstream sValue;
		sValue << genes;
		xGEPChromosome.addAttribute("genes", sValue.str().c_str());
		sValue.str("");
		sValue << headLength;
		xGEPChromosome.addAttribute("headLength",sValue.str().c_str());
		sValue.str("");
		sValue << tailLength;
		xGEPChromosome.addAttribute("tailLength", sValue.str().c_str());
		sValue.str("");
		sValue << linkHeadLength;
		xGEPChromosome.addAttribute("linkLength", sValue.str().c_str());
		for (uint g = 0; g < genes; g++){
			sValue.str("");
			XMLNode xGene = XMLNode::createXMLTopNode("Gene");
			for (uint i = 0; i < this->geneLength; i++) {
				sValue << this->at(g*(this->geneLength)+i)->primitive_->getName() << " ";
			}
			xGene.addText(sValue.str().c_str());
			xGEPChromosome.addChild(xGene);
		}
		// print homeotic gene
			sValue.str("");
			XMLNode xCell = XMLNode::createXMLTopNode("Cell");
			uint cellOffset = this->genes * this->geneLength;
			for (uint i = 0; i < this->linkHeadLength + this->linkTailLength; i++) {
				sValue << this->at(cellOffset + i)->primitive_->getName() << " ";
			}
			xCell.addText(sValue.str().c_str());
			xGEPChromosome.addChild(xCell);
	}


	// read from XMLNode
	// mandatory if running parallel ECF or reading population from a milestone file
	void GEPChromosome::read(XMLNode& xGEPChromosome)
	{
		// temporary comment-out, to avoid access to primitiveSet_->primitives_
		return;

/*
		this->clear();
		//this->primitiveSet_ = static_cast<Tree::PrimitiveSetP> (new Tree::PrimitiveSet);
		//this->primitiveSet_->initialize(state_);
		XMLCSTR genesStr = xGEPChromosome.getAttribute("genes");
		uint size = str2uint(genesStr);

		XMLCSTR hlenStr = xGEPChromosome.getAttribute("headLength");
		uint headlen = str2uint(hlenStr);

		XMLCSTR tlenStr = xGEPChromosome.getAttribute("linkLength");
		uint linklen = str2uint(tlenStr);
		// loop over genes
		for (uint i = 0; i <= size; i++){
			XMLNode xGene = xGEPChromosome.getChildNode(i);
			XMLCSTR tree = xGene.getText();
			std::stringstream stream;
			stream << tree;

			if (i < size){
				std::vector<Tree::PrimitiveP>& primitives = primitiveSet_->primitives_;
				std::string primitiveStr;
				uint position = 0;

				for (uint iNode = 0; iNode < this->geneLength; iNode++) {
					stream >> primitiveStr;
					Tree::Node* node = new Tree::Node();

					// 'regular' primitives
					Tree::PrimitiveP prim = primitiveSet_->getPrimitiveByName(primitiveStr);
					if (prim != Tree::PrimitiveP()) {
						node->setPrimitive(prim);
						this->push_back(static_cast<Tree::NodeP>(node));
						continue;
					}
					// ERCs
					// (TODO: include user defined ERC types)
					Tree::PrimitiveP erc;
					std::string prefix = primitiveStr.substr(0, 2);
					std::string value = primitiveStr.substr(2);
					std::stringstream ss;
					ss << value;
					if (prefix == DBL_PREFIX) {
					erc = (Tree::PrimitiveP)(new Tree::Primitives::ERCD);
					double v;
					ss >> v;
					erc->setValue(&v);
					}
					else if (prefix == INT_PREFIX) {
					erc = (Tree::PrimitiveP)(new Tree::Primitives::ERC<int>);
					int v;
					ss >> v;
					erc->setValue(&v);
					}
					else if (prefix == BOOL_PREFIX) {
					erc = (Tree::PrimitiveP)(new Tree::Primitives::ERC<bool>);
					bool v;
					ss >> v;
					erc->setValue(&v);
					}
					else if (prefix == CHR_PREFIX) {
					erc = (Tree::PrimitiveP)(new Tree::Primitives::ERC<char>);
					char v;
					ss >> v;
					erc->setValue(&v);
					}
					else if (prefix == STR_PREFIX) {
					erc = (Tree::PrimitiveP)(new Tree::Primitives::ERC<std::string>);
					std::string v;
					ss >> v;
					erc->setValue(&v);
					}
					else {
						ECF_LOG_ERROR(state_, "GEPChromosome genotype: undefined primitive (" + primitiveStr + ")!");
						throw("");
					}
					erc->setName(primitiveStr);
					node->primitive_ = erc;
					this->push_back(static_cast<Tree::NodeP>(node));
				}
			}
			else{ // Deal with the "Cell" gene
				std::vector<Tree::PrimitiveP>& primitives = linkFunctionSet_->primitives_;
				std::string primitiveStr;
				uint position = 0;

				for (uint iNode = 0; iNode < this->linkHeadLength+this->linkTailLength; iNode++) {
					stream >> primitiveStr;
					Tree::Node* node = new Tree::Node();
					// 'regular' primitives
					Tree::PrimitiveP prim = linkFunctionSet_->getPrimitiveByName(primitiveStr);
					if (prim != Tree::PrimitiveP()) { // if it is a linking function
						node->setPrimitive(prim);
						this->push_back(static_cast<Tree::NodeP>(node));
						continue;
					}
					else{
							ECF_LOG_ERROR(state_, "GEPChromosome genotype: undefined primitive (" + primitiveStr + ") for the Cell gene!");
							throw("");
						}
					}
				}
			}
			*/
	}

	Tree::Tree* GEPChromosome::toTree(uint gene)
	{
		ECF_LOG(this->state_, 5, "Performing GEP -> Tree conversion...");

		Tree::Tree* tree = new Tree::Tree();
		// Copy primitive set
		tree->primitiveSet_ = this->primitiveSet_;

		uint geneOffset = gene*(this->geneLength);
		uint ercIdx = geneOffset + this->headLength + this->tailLength;
		uint ercCount = 0;
		// Get root node arity
		//geneOffset = 0; 
		uint i = geneOffset;
		uint nArgs = this->at(i++)->primitive_->getNumberOfArguments();
		// Get tree level indices
		std::vector<uint> idx;
		int level = 0;
		uint nextLevelStart = 1 + geneOffset;
		idx.push_back(geneOffset);
		while (nArgs > 0){
			uint lvlArity = 0;
			idx.push_back(nextLevelStart);
			for (uint j = 0; j < nArgs; j++){
				lvlArity += this->at(nextLevelStart++)->primitive_->getNumberOfArguments();
			}
			nArgs = lvlArity;
		}
		//Read the gene and annotate the locations of the constants (needed later)
		std::vector<int> constants(this->size(), -99999);
		if (this->dcLength > 0){
			for (uint c = geneOffset; c < geneOffset + this->headLength + this->tailLength; c++){
				if (this->at(c)->primitive_->getName() == "?"){
					constants[c] = ercCount++;
				}
			}
		}
		// Translate expression
		// Helper array to store the per-level arguments needed
		std::vector<uint> args(idx.size(), 0);
		// Iterate while root node hasn't completed
		std::vector<bool> visited(this->size(), false);
		while (idx.at(0) == geneOffset){
			// Read and this node to GP expression, if it hasn't been visited yet
			if (!visited.at(idx.at(level))){
				Tree::NodeP GEPnode = static_cast<Tree::NodeP> (new Tree::Node(this->at(idx.at(level))));
				// If it is an ERC placeholder, replace with the next ERC
				if (GEPnode->primitive_->getName() == "?"){
					GEPnode = static_cast<Tree::NodeP> (new Tree::Node(this->at(ercIdx+constants.at(idx.at(level)))));
				}
				args[level] = GEPnode->primitive_->getNumberOfArguments();
				// Push node into Tree representation
				Tree::NodeP node = static_cast<Tree::NodeP> (new Tree::Node(GEPnode));
				tree->addNode(node);
				visited.at(idx.at(level)) = true;
			}
			// If operator still needs children, go down one level and read the necessary arguments
			if (args.at(level) > 0){
				level++;
			}
			// If it is a terminal or a satisfied operator, go up one level and increase reading index
			else{
				idx[level]++;
				level--;
				// Decrease needed arguments
				if (level >= 0) args[level]--;
			}
		}
		// Update the size and depth of each tree node
		tree->update();
		// Print tree
		XMLNode xInd;
		tree->write(xInd);
		char *s = xInd.createXMLString();
		ECF_LOG(this->state_, 5, "Tree conversion result: \n" + std::string(s));
		freeXMLString(s);
		return tree;
	}

	Tree::Tree* GEPChromosome::makeCellTree()
	{
		ECF_LOG(this->state_, 5, "Performing GEP -> Tree conversion at the cell level...");

		Tree::Tree* tree = new Tree::Tree();
		// Copy primitive set
		tree->primitiveSet_ = this->linkFunctionSet_;

		uint geneOffset = this->genes*(this->geneLength);
		// Get root node arity
		uint i = geneOffset;
		uint nArgs = this->at(i++)->primitive_->getNumberOfArguments();
		// Get tree level indices
		std::vector<uint> idx;
		int level = 0;
		uint nextLevelStart = 1 + geneOffset;
		idx.push_back(geneOffset);
		while (nArgs > 0){
			uint lvlArity = 0;
			idx.push_back(nextLevelStart);
			for (uint j = 0; j < nArgs; j++){
				lvlArity += this->at(nextLevelStart++)->primitive_->getNumberOfArguments();
			}
			nArgs = lvlArity;
		}
		// Translate expression
		// Helper array to store the per-level arguments needed
		std::vector<uint> args(idx.size(), 0);
		// Iterate while root node hasn't completed
		std::vector<bool> visited(this->size(), false);
		while (idx.at(0) == geneOffset){
			// Read and this node to GP expression, if it hasn't been visited yet
			if (!visited.at(idx.at(level))){
				Tree::NodeP GEPnode = static_cast<Tree::NodeP> (new Tree::Node(this->at(idx.at(level))));
				args[level] = GEPnode->primitive_->getNumberOfArguments();
				// Push node into Tree representation
				Tree::NodeP node = static_cast<Tree::NodeP> (new Tree::Node(GEPnode));
				tree->addNode(node);
				visited.at(idx.at(level)) = true;
			}
			// If operator still needs children, go down one level and read the necessary arguments
			if (args.at(level) > 0){
				level++;
			}
			// If it is a terminal or a satisfied operator, go up one level and increase reading index
			else{
				idx[level]++;
				level--;
				// Decrease needed arguments
				if (level >= 0) args[level]--;
			}
		}
		// Update the size and depth of each tree node
		tree->update();
		// Print tree
		XMLNode xInd;
		tree->write(xInd);
		char *s = xInd.createXMLString();
		ECF_LOG(this->state_, 5, "Tree conversion result: \n" + std::string(s));
		freeXMLString(s);
		return tree;
	}
	void GEPChromosome::assemble(){
		this->subtrees.clear();
		this->cellTree = this->makeCellTree();
		for (uint i = 0; i < this->genes; i++){
			Tree::Tree *subtree = this->toTree(i);
			this->subtrees.push_back(subtree);
		}
	}

	void GEPChromosome::execute(void *result)
	{ 
		// Obtain the cell tree structure
		//Tree::Tree *tree = this->cellTree();
		// Translate and execute all the gene subtrees
		// TODO: detect which genes are actually used so as to not evaluate unneeded ones
		double tmp = 0;
		for (uint i = 0; i < this->genes; i++){
			Tree::Tree *subtree = this->subtrees.at(i);
			subtree->execute(&tmp);
			// Set the terminal values according to the subtrees
			this->cellTree->setTerminalValue(GEP_GENE_PREFIX + uint2str(i), &tmp);
		}
		// Finally, translate the cell tree and store the result
		this->cellTree->execute(result);
	}

	/**
	* \brief Set a terminal's value.
	*
	* \param name	terminal's name
	* \param value	terminal's value
	*/
	void GEPChromosome::setTerminalValue(std::string name, void* value)
	{
		Tree::PrimitiveP term = primitiveSet_->getTerminalByName(name);
		if (term == Tree::PrimitiveP()) {
			ECF_LOG_ERROR(state_, "GEPChromosome genotype: invalid terminal name referenced in setTerminalValue()!");
			throw("");
		}

		term->setValue(value);
	}
}

