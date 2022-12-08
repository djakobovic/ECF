#include "ECF.h"
#include <fstream>
#include <iomanip>
#include <time.h>


/**
 * \brief Construct the one and only State object.
 *
 * Also register all known algorithms, genotypes and termination operators, create the population and some other elements of the system.
 */
State::State()
{
	this->population_ = static_cast<PopulationP> (new Population);
	this->crossover_ = static_cast<CrossoverP> (new Crossover);
	this->mutation_ = static_cast<MutationP> (new Mutation);
	this->context_ = static_cast<EvolutionContextP> (new EvolutionContext);

	XMLNode::setGlobalOptions(XMLNode::char_encoding_legacy);	// XML encoding

	bInitialized_ = false;
	bCommandLine_ = false;
	bAlgorithmSet_ = false;
	bGenotypeSet_ = false;
	bEvaluatorSet_ = false;
	bLoadMilestone_ = false;
	bBatchMode_ = false;
	bBatchStart_ = false;
	bBatchSingleMilestone_ = false;
	bBatchWriteStats_ = false;

	// register existing components:
	// algorithms
	AlgorithmP alg = static_cast<AlgorithmP> (new SteadyStateTournament);
	this->mAlgorithms_[alg->getName()] = alg;
	alg = static_cast<AlgorithmP> (new RouletteWheel);
	this->mAlgorithms_[alg->getName()] = alg;
	alg = static_cast<AlgorithmP> (new ParticleSwarmOptimization);
	this->mAlgorithms_[alg->getName()] = alg;
	alg = static_cast<AlgorithmP> (new Elimination);
	this->mAlgorithms_[alg->getName()] = alg;
	alg = static_cast<AlgorithmP> (new XCS);
	this->mAlgorithms_[alg->getName()] = alg;
	alg = static_cast<AlgorithmP> (new RandomSearch);
	this->mAlgorithms_[alg->getName()] = alg;
	alg = static_cast<AlgorithmP> (new GeneticAnnealing);
	this->mAlgorithms_[alg->getName()] = alg;
	alg = static_cast<AlgorithmP> (new DifferentialEvolution);
	this->mAlgorithms_[alg->getName()] = alg;
	alg = static_cast<AlgorithmP> (new ArtificialBeeColony);
	this->mAlgorithms_[alg->getName()] = alg;
	alg = static_cast<AlgorithmP> (new GenHookeJeeves);
	this->mAlgorithms_[alg->getName()] = alg;
	alg = static_cast<AlgorithmP> (new Clonalg);
	this->mAlgorithms_[alg->getName()] = alg;
	alg = static_cast<AlgorithmP> (new OptIA);
	this->mAlgorithms_[alg->getName()] = alg;
	alg = static_cast<AlgorithmP> (new EvolutionStrategy);
	this->mAlgorithms_[alg->getName()] = alg;
	alg = static_cast<AlgorithmP> (new AlgNSGA2);
	this->mAlgorithms_[alg->getName()] = alg;
	alg = static_cast<AlgorithmP> (new CuckooSearch);
	this->mAlgorithms_[alg->getName()] = alg;
	alg = static_cast<AlgorithmP> (new AlgGEP);
	this->mAlgorithms_[alg->getName()] = alg;

#ifdef _MPI
	// paralel algorithms
	alg = static_cast<AlgorithmP> (new AlgSGenGpea);
	this->mAlgorithms_[alg->getName()] = alg;
	alg = static_cast<AlgorithmP> (new AlgAEliGpea);
	this->mAlgorithms_[alg->getName()] = alg;
	alg = static_cast<AlgorithmP> (new AlgAEliGpea2);
	this->mAlgorithms_[alg->getName()] = alg;
#endif

	// genotypes
	GenotypeP gen = static_cast<GenotypeP> (new BitString::BitString);
	this->mGenotypes_[gen->getName()] = gen;
	gen = static_cast<GenotypeP> (new Binary::Binary);
	this->mGenotypes_[gen->getName()] = gen;
	gen = static_cast<GenotypeP> (new Tree::Tree);
	this->mGenotypes_[gen->getName()] = gen;
	gen = static_cast<GenotypeP> (new Permutation::Permutation);
	this->mGenotypes_[gen->getName()] = gen;
	gen = static_cast<GenotypeP> (new FloatingPoint::FloatingPoint);
	this->mGenotypes_[gen->getName()] = gen;
	gen = static_cast<GenotypeP> (new Tree::APGenotype);
	this->mGenotypes_[gen->getName()] = gen;
	gen = static_cast<GenotypeP> (new IntGenotype::IntGenotype);
	this->mGenotypes_[gen->getName()] = gen;
	gen = static_cast<GenotypeP> (new GEP::GEPChromosome);
	this->mGenotypes_[gen->getName()] = gen;
	//	gen = static_cast<GenotypeP> (new cart::Cartesian);
//	this->mGenotypes_[gen->getName()] = gen;

	// termination operators
	OperatorP op = static_cast<OperatorP> (new TermStagnationOp);
	this->allTerminationOps_.push_back(op);
	op = static_cast<OperatorP> (new TermMaxGenOp);
	this->allTerminationOps_.push_back(op);
	op = static_cast<OperatorP> (new TermFitnessValOp);
	this->allTerminationOps_.push_back(op);
	op = static_cast<OperatorP> (new TermMaxTimeOp);
	this->allTerminationOps_.push_back(op);
	op = static_cast<OperatorP> (new TermMaxEvalOp);
	this->allTerminationOps_.push_back(op);

	setRandomizer(static_cast<RandomizerP> (new SimpleRandomizer));
	this->registry_ = static_cast<RegistryP> (new Registry);
	this->logger_ = static_cast<LoggerP> (new Logger);
	this->comm_ = static_cast<CommunicatorP> (new Comm::Communicator);
	this->migration_ = static_cast<MigrationP> (new Migration);
}


/**
 * \brief	Register parameters of framework components.
 * Called by: State::initialize
 */
void State::registerParameters()
{
	// State parameters
	registry_->registerEntry("milestone.interval", (voidP) (new uint(0)), ECF::UINT,
		"milestone saving interval in generations; 0: save only at the end (default: 0)");
	registry_->registerEntry("milestone.filename", (voidP) (new std::string("milestone.txt")), ECF::STRING,
		"milestone file (if stated) stores all the population (default: none)");
	registry_->registerEntry("batch.repeats", (voidP) (new uint(0)), ECF::UINT,
		"number of independent runs to perform (default: 1)");
	registry_->registerEntry("batch.singlemilestone", (voidP) (new uint(0)), ECF::UINT,
		"use only one milestone file for all the batch runs (1) or one for each run (0) (default: 0)");
	registry_->registerEntry("batch.statsfile", (voidP) (new std::string("")), ECF::STRING,
		"output batch end of run stats in a single file (default: none)");

	// milestone data
	registry_->registerEntry("milestone.generation_", (voidP) (new uint(0)), ECF::UINT);
	registry_->registerEntry("milestone.elapsedtime_", (voidP) (new uint(0)), ECF::UINT);
	registry_->registerEntry("batch.remaining_", (voidP) (new uint(0)), ECF::UINT);
	registry_->registerEntry("batch.logfile_", (voidP) (new std::string("")), ECF::STRING);

	ECF_LOG(this, 4, "Registering parameters: algorithms, operators");

	// call registerParameters() methods:
	alg_iter itAlg;
	for(itAlg = mAlgorithms_.begin(); itAlg != mAlgorithms_.end(); ++itAlg)
		itAlg->second->registerParameters(state_);

	// register common implicit parallel parameters (defined in Algorithm base class)
	mAlgorithms_.begin()->second->registerParallelParameters(state_);

	// register termination operators' parameters
	for(uint i = 0; i < allTerminationOps_.size(); i++)
		allTerminationOps_[i]->registerParameters(state_);

	// user-defined operators
	for(uint i = 0; i < allUserOps_.size(); i++)
		allUserOps_[i]->registerParameters(state_);

	mutation_->registerParameters(state_);
	crossover_->registerParameters(state_);
	randomizer_->registerParameters(state_);
	population_->registerParameters(state_);
	logger_->registerParameters(state_);
	migration_->registerParameters(state_);
	evalOp_->registerParameters(state_);
}


/**
 * \brief	Read State parameters from the Registry.
 * Called by: State::initialize
 */
void State::readParameters()
{
	ECF_LOG(this, 4, "Rading parameters from the Registry");

	// milestone saving data
	if(registry_->isModified("milestone.filename"))
		bSaveMilestone_ = true;
	else
		bSaveMilestone_ = false;

	voidP sptr = registry_->getEntry("milestone.interval");
	milestoneInterval_ = *((uint*) sptr.get());

	sptr = registry_->getEntry("milestone.filename");
	milestoneFilename_ = *((std::string*) sptr.get());

	// milestone loading data
	if(registry_->isModified("milestone.generation_"))
		bLoadMilestone_ = true;
	else
		bLoadMilestone_ = false;

	sptr = registry_->getEntry("milestone.generation_");
	milestoneGeneration_ = *((uint*) sptr.get());

	sptr = registry_->getEntry("milestone.elapsedtime_");
	milestoneElapsedTime_ = *((uint*) sptr.get());

	// batch running data
	sptr = registry_->getEntry("batch.repeats");
	batchRepeats_ = *((uint*) sptr.get());

	sptr = registry_->getEntry("batch.remaining_");
	batchRemaining_ = *((uint*) sptr.get());

	sptr = registry_->getEntry("batch.statsfile");
	batchStatsFile_ = *((std::string*) sptr.get());

	sptr = registry_->getEntry("batch.logfile_");
	batchLogFile_ = *((std::string*) sptr.get());

	sptr = registry_->getEntry("batch.singlemilestone");
	bBatchSingleMilestone_ = (*((uint*) sptr.get()) % 2) ? true:false;

	if(registry_->isModified("batch.repeats") && batchRepeats_ > 1)
		bBatchStart_ = true;
	else
		bBatchStart_ = false;
}


/**
 * \brief	Dump all parameters in file.
 */
void State::dumpParameters(std::string fileName, bool addClear)
{
	XMLNode xMainNode = XMLNode::createXMLTopNode("ECF");
	xMainNode.addAttribute("version", ECF_VERSION.c_str());
	xMainNode.addClear("ECF parameter dump - list of all parameters", "<!-- ", " -->");

	this->state_ = this->getState(); // obtain a sptr to this
	// register all parameters (except genotypes which are normally parsed from config)
	this->registerParameters();

	// register parameters for all the genotypes
	for(gen_iter itGen = mGenotypes_.begin(); itGen != mGenotypes_.end(); ++itGen) {
		itGen->second->setGenotypeId(0);
		itGen->second->registerParameters(state_);

		// register all crx operators
		std::vector<CrossoverOpP> crx = itGen->second->getCrossoverOp();
		for(uint iOp = 0; iOp < crx.size(); iOp++) {
			crx[iOp]->myGenotype_ = itGen->second;
			crx[iOp]->registerParameters(state_);
		}

		// register all mutation operators
		std::vector<MutationOpP> mut = itGen->second->getMutationOp();
		for(uint iOp = 0; iOp < mut.size(); iOp++) {
			mut[iOp]->myGenotype_ = itGen->second;
			mut[iOp]->registerParameters(state_);
		}
	}

	// get all parameters
	XMLNode xRegistry;
	this->registry_->dumpEntries(xRegistry);

	// create Algorithm node
	XMLNode xAlgorithms = XMLNode::createXMLTopNode(NODE_ALGORITHM);
	xMainNode.addChild(xAlgorithms);

	// extract all algorithms
	alg_iter itAlg;
	for(itAlg = mAlgorithms_.begin(); itAlg != mAlgorithms_.end(); ++itAlg) {
		
		// get algorithm name, create subnode
		std::string algorithmName = itAlg->first;
		XMLNode xAlgorithm = XMLNode::createXMLTopNode(algorithmName.c_str());
		xAlgorithms.addChild(xAlgorithm);

		// iterate over all entries
		for(int i = 0; i < xRegistry.nChildNode(); i++) {
			XMLNode child = xRegistry.getChildNode(i);
			std::string key = child.getAttributeValue();
			// compare to selected Algorithm
			if(key.compare(0, algorithmName.size(), algorithmName) == 0) {
				// copy entry, delete the original
				XMLNode xEntry = child.deepCopy();
				child.deleteNodeContent();
				i--;

				// remove algorithm name
				std::string key = xEntry.getAttribute("key");
				key.erase(0, algorithmName.length() + 1);
				xEntry.updateAttribute(key.c_str(), NULL, "key");
				xAlgorithm.addChild(xEntry);

				// optional: copy description attribute to XML comment
				std::string desc = xEntry.getAttribute("desc");
				if(addClear && desc != "") {
					xAlgorithm.addClear(desc.c_str(), "<!-- ", " -->");
				}
				if(addClear)
					xEntry.deleteAttribute("desc");
			}
		}
	}

	// create Genotype node
	XMLNode xGenotypes = XMLNode::createXMLTopNode(NODE_GENOTYPE);
	xMainNode.addChild(xGenotypes);

	// extract all genotypes
	for(gen_iter itGen = mGenotypes_.begin(); itGen != mGenotypes_.end(); ++itGen) {
		
		// get genotype name, create subnode
		std::string genotypeName = itGen->first;
		XMLNode xGenotype = XMLNode::createXMLTopNode(genotypeName.c_str());
		xGenotypes.addChild(xGenotype);

		// iterate over all entries
		for(int i = 0; i < xRegistry.nChildNode(); i++) {
			XMLNode child = xRegistry.getChildNode(i);
			std::string key = child.getAttributeValue();
			// compare to selected Algorithm
			if(key.compare(0, genotypeName.size(), genotypeName) == 0) {
				// copy entry, delete the original
				XMLNode xEntry = child.deepCopy();
				child.deleteNodeContent();
				i--;

				// remove algorithm name
				std::string key = xEntry.getAttribute("key");
				key.erase(0, genotypeName.length() + 1);
				xEntry.updateAttribute(key.c_str(), NULL, "key");
				xGenotype.addChild(xEntry);

				// optional: copy description attribute to XML comment
				std::string desc = xEntry.getAttribute("desc");
				if(addClear && desc != "") {
					xGenotype.addClear(desc.c_str(), "<!-- ", " -->");
				}
				if(addClear)
					xEntry.deleteAttribute("desc");
			}
		}
	}

	// optional: copy description for all registry entries
	if(addClear) {
		XMLNode xRegClear = XMLNode::createXMLTopNode(NODE_REGISTRY);
		for(int i = 0; i < xRegistry.nChildNode(); i++) {
			XMLNode xEntry = xRegistry.getChildNode(i);
			XMLNode xE2 = xEntry.deepCopy();
			std::string desc = xEntry.getAttribute("desc");
			xE2.deleteAttribute("desc");
			xRegClear.addChild(xE2);
			if(desc != "")
				xRegClear.addClear(desc.c_str(), "<!-- ", " -->");
		}
		xMainNode.addChild(xRegClear);
	}
	else
		// add remaining Registry entries
		xMainNode.addChild(xRegistry);

	// user request
	if(addClear)
		xMainNode.writeToFile(fileName.c_str());
	// GUI request
	else {
		std::string output(xMainNode.createXMLString());
		std::cout << output;
	}
}



/**
 * \brief	Parse given config file.
 * Called by: State::initialize
 * \param filename	config file name
 */
bool State::parseConfig(std::string filename) 
{
	std::ifstream fin(filename.c_str());
	if (!fin) {
		throw std::string("Error opening file " + filename);
	}
	std::cout << "Parsing configuration file: " << filename << std::endl;

	std::string xmlFile, temp;
	while (!fin.eof()) {
		getline(fin, temp);
		xmlFile += "\n" + temp;
	}

	XMLResults results;
	xConfig_ = XMLNode::parseString(xmlFile.c_str(), "ECF", &results);
	if (results.error != eXMLErrorNone) {
		std::cout << "Configuration file: " << XMLNode::getError(results.error);
		std::cout << " (line " << results.nLine << ", col " << results.nColumn << ")" << std::endl;
		throw("");
	}

	if (xConfig_.isEmpty())
		return false;

	int n = xConfig_.nChildNode();
	for (int i = 0; i < n; ++i) {
		XMLNode     child = xConfig_.getChildNode(i);
		std::string name  = child.getName();
		bool        ok    = true;

		if (name == NODE_REGISTRY) 
			ok &= registry_->readEntries(child);
		else if (name == NODE_ALGORITHM)
			ok &= parseAlgorithmNode(child);
		else if (name == NODE_GENOTYPE)
			ok &= parseGenotypeNode(child);
		else if (name == NODE_POPULATION)
			continue;
		else 
			std::cout << "Unknown node: " << name << std::endl;

		if (!ok)
			throw "";
	}

	return true;
}


/**
 * \brief	Parse Algorithm node from config.
 * Called by: State::parseConfig
 * \param node	current XML node
 */
bool State::parseAlgorithmNode(XMLNode node)
{
	int n = node.nChildNode();
	if (n > 1)
		std::cout << "Warning: multiple Algorithm nodes found! (using the first one)" << std::endl;

	XMLNode child = node.getChildNode(0);
	alg_iter alg = mAlgorithms_.find(child.getName());
	if (alg == mAlgorithms_.end()) {
		throw std::string("Error: unknown Algorithm : ") + child.getName();
	}

	algorithm_ = alg->second;
	bAlgorithmSet_ = true;

	if (! registry_->readEntries(child, child.getName()))
		return false;

	return true;
}


/**
 * \brief	Parse Genotype node from config.
 * Called by: State::parseConfig
 * \param node	current XML node
 */
bool State::parseGenotypeNode(XMLNode node)
{
	int n = node.nChildNode();
	for (int i = 0; i < n; ++i) {
		XMLNode child = node.getChildNode(i);
		gen_iter gen = mGenotypes_.find(child.getName());
		if (gen == mGenotypes_.end()) {
			throw std::string("Error: unknown Genotype : ") + child.getName();
		}

		uint genotypeId = (uint)genotype_.size();
		gen->second->setGenotypeId(genotypeId);
		gen->second->registerParameters(state_);
		setGenotype(gen->second);

		if (!registry_->readEntries(child, child.getName(), genotypeId))
			return false;
	}

	return true;
}


/**
 * \brief Initialize system components.
 * 
 * Performed after parameter parsing.
 * Called by: State::initialize
 */
bool State::initializeComponents(int argc, char **argv)
{
	try {
		// initialize evolutionary context
		context_->initialize();

		if(!bBatchStart_)
			logger_->initialize(state_);
		randomizer_->initialize(state_);

		// initialize single instance of active genotypes
		// (active genotypes - the ones that the individuals are made of, which is defined in the configuration file)
		// State keeps a single uninitialized object of all active Genotypes
		ECF_LOG(this, 4, "Initializing active genotypes...");
		for(uint i = 0; i < genotype_.size(); i++) {
			GenotypeP copy = (GenotypeP) genotype_[i]->copy();
			bInitialized_ &= copy->initialize(state_);
		}
		if(!bInitialized_) {
			throw "Error: Genotype initialization failed!";
		}

		// MPI communicator
		bInitialized_ &= comm_->initialize(state_, argc, argv);

		// damo algoritmu pointer na operator evaluacije i pointere na repozitorij krizanja i mutacije
		ECF_LOG(this, 4, "Initializing population and algorithm...");
		algorithm_->evalOp_ = this->evalOp_;
		algorithm_->crossover_ = crossover_;
		algorithm_->mutation_ = mutation_;
		algorithm_->state_ = state_;
		algorithm_->initialize(state_);	

		population_->initialize(state_);
		algorithm_->initializeParallel(state_);	// provjera impl. paralelizacije (nakon population_->initialize zbog podjele procesa po demovima)

		ECF_LOG(this, 4, "Initializing genetic operators...");
		mutation_->initialize(state_);
		crossover_->initialize(state_);
		migration_->initialize(state_);

		// initialize termination ops
		ECF_LOG(this, 4, "Initializing termination operators...");
		activeTerminationOps_.clear();
		for(uint i = 0; i < allTerminationOps_.size(); i++)
			if(allTerminationOps_[i]->initialize(state_))
				activeTerminationOps_.push_back(allTerminationOps_[i]);
		// if no term operators are configured, activate default (the first)
		if(activeTerminationOps_.empty())
			activeTerminationOps_.push_back(allTerminationOps_[0]);

		// initialize user ops
		ECF_LOG(this, 4, "Initializing user defined operators...");
		activeUserOps_.clear();
		for(uint i = 0; i < allUserOps_.size(); i++)
			if(allUserOps_[i]->initialize(state_))
				activeUserOps_.push_back(allUserOps_[i]);

		// evaluation op. initializes last
		ECF_LOG(this, 4, "Initializing evaluation operator...");
		if(!evalOp_->initialize(state_))
			throw "Error: Evaluation operator initialization failed!";

		// generate one individual
		ECF_LOG(this, 4, "Generating test individual...");
		individual_ = (IndividualP) (new Individual(state_));

	} // try

	catch(std::string& msg) {
		std::cout << msg << std::endl;
		bInitialized_ = false;
	}
	catch(const char* msg) {
		std::cout << msg << std::endl;
		bInitialized_ = false;
	}
	catch(...) {
		std::cout << "Unknown error in initialization!" << std::endl;
		bInitialized_ = false;
	}

	return bInitialized_;
}


/// get one initial Fitness object (create on demand)
FitnessP State::getFitnessObject()
{	
	try {
		if(!fitness_) {
			context_->evaluatedIndividual = individual_;
			fitness_ = evalOp_->evaluate(individual_);
		}

	}
	catch(...) {
		std::cout << "Error in evaluation operator!" << std::endl;
	}

	return fitness_;
}


/**
 * \brief Runs repeatedly (in batch mode).
 * 
 * The key parameter is 'batch.repeats'
 * Called by: State::run()
 */
bool State::runBatch()
{
	bBatchStart_ = false;
	bBatchMode_ = true;

	bool bUseLog = registry_->isModified("log.filename");

	// are we restoring from a milestone file
	if(!bLoadMilestone_)
		batchRemaining_ = batchRepeats_;
	uint numerals = 1 + (uint) (log((double) batchRepeats_) / log((double) 10.));

	// get logfile name and extension
	std::string logFileName = *(std::string*) registry_->getEntry("log.filename").get();
	std::string logFileExt = "";
	if(bLoadMilestone_)
		logFileName = batchLogFile_;
	else
		batchLogFile_ = logFileName;

	if(logFileName.find_last_of(".") != std::string::npos) {
		logFileExt = logFileName.substr(logFileName.find_last_of("."));
		logFileName = logFileName.substr(0, logFileName.find_last_of("."));
	}

	// get milestone name and extension
	std::string milestoneName = *(std::string*) registry_->getEntry("milestone.filename").get();
	std::string milestoneExt = "";
	if(milestoneName.find_last_of(".") != std::string::npos) {
		milestoneExt = milestoneName.substr(milestoneName.find_last_of("."));
		milestoneName = milestoneName.substr(0, milestoneName.find_last_of("."));
	}

	// (re)open stats file, if defined
	std::ofstream statsFile;
	if(registry_->isModified("batch.statsfile")) {
		if(bLoadMilestone_)
			statsFile.open(batchStatsFile_.c_str(), std::ios_base::app);
		else {
			statsFile.open(batchStatsFile_.c_str());
			statsFile << "runId\tfit_min\tfit_max\tfit_avg\tfit_std\t#evals\ttime\tgen\n";
		}
		if(!statsFile) {
			ECF_LOG_ERROR(this, "Error: can't open batch statsfile (" + batchStatsFile_ + ")!");
			return false;
		}
		statsFile.close();
		bBatchWriteStats_ = true;
	}

	uint runId = batchRepeats_ - batchRemaining_ + 1;

	// perform algorithm runs
	for(; runId <= batchRepeats_; runId++) {
		// set current logfile
		if(bUseLog) {
			std::stringstream ss;
			ss << std::setw(numerals) << std::setfill('0') << runId;
			std::string currentLogName = logFileName + "_" + ss.str() + logFileExt;
			registry_->modifyEntry("log.filename", (voidP) new std::string(currentLogName));
		}

		// set current milestone
		if(!bBatchSingleMilestone_) {
			std::stringstream ss;
			ss << std::setw(numerals) << std::setfill('0') << runId;
			milestoneFilename_ = milestoneName + "_" + ss.str() + milestoneExt;
		}

		// run
		bInitialized_ = true;
		if(!initializeComponents(argc_, argv_))
			break;
		ECF_LOG(this, 1, "Running in batch mode: run " + uint2str(runId) + "/" + uint2str(batchRepeats_));
		run();
		batchRemaining_--;

		// write stats (PECF: only master process)
		if(comm_->getCommGlobalRank() == 0) {
			if(bBatchWriteStats_) {
				statsFile.open(batchStatsFile_.c_str(), std::ios_base::app);
				std::vector<double> stats = population_->getStats()->getStats();
				statsFile << runId << '\t';
				statsFile << stats[ECF::FIT_LOW] << '\t' << stats[ECF::FIT_HIGH] << '\t' << stats[ECF::FIT_AVG] << '\t' << stats[ECF::FIT_DEV] << '\t';
				statsFile << stats[ECF::STAT_EVAL] << '\t' << stats[ECF::STAT_TIME] << '\t' << getGenerationNo() << '\n';
				statsFile.close();
			}
		}
	}

	// ugly workabout for MPI_Finalize, which may only be called once
	// (shouldn't be called at the end of a single run)
	bBatchMode_ = false;
	comm_->finalize();

	if(bInitialized_)
		std::cout << "Batch mode end (" << batchRepeats_ << " runs concluded)." << std::endl;

	if(statsFile)
		statsFile.close();
	return true;
}



/**
 * \brief Initialize the whole system.
 * 
 * Initialization steps:
 * - parse command line arguments
 * - register parameters of framework components
 * - parse config file
 * - initialize all components.
 * Called by: ::main
 */
bool State::initialize(int argc, char **argv)
{
	this->state_ = this->getState(); // obtain a sptr to this

	genotype_.clear();
	mutation_->operators.clear();
	crossover_->operators.clear();

	bInitialized_ = false;
	argc_ = argc;
	argv_ = argv;
	std::string config_file;

	try {

		std::cout << "-- ECF, version " << ECF_VERSION << " --" << std::endl;

		if(!bEvaluatorSet_) {
			throw "Error: no EvaluateOp defined!";
		}

		if (argc > 1) {
			// parse arguments: return success if command line option is recognized
			if(parseCommandLine(argc, argv))
				return false;

			// otherwise, assume configuration file name as argument
			config_file = argv[1];
		}

		registerParameters();

		if (config_file != "") {
			parseConfig(config_file);
		}
		else {
			std::cout << "Warning: no configuration file given." << std::endl;
			std::cout << "Example usage: <ECF_executable> <parameter_file>" << std::endl;
		}

		// use the default algorithm
		if (!bAlgorithmSet_)
			algorithm_ = mAlgorithms_.find("SteadyStateTournament")->second;

		if(!bGenotypeSet_) {
			throw "Error: no Genotype defined!";
		}

		readParameters();

		// set init flag, then test with each component initialization
		bInitialized_ = true;

		initializeComponents(argc, argv);

		// if multiple runs, milestone will get parsed in State::run()
		if (bBatchStart_)
			return true;

		if(bLoadMilestone_)
			loadMilestone();

		ECF_LOG(this, 4, "Initialization complete.");

	} // try

	catch(std::string& msg) {
		std::cout << msg << std::endl;
	}
	catch(const char* msg) {
		std::cout << msg << std::endl;
	}
	catch(...) {
		std::cout << "Unknown error in initialization!" << std::endl;
	}

	return bInitialized_;
}



/**
 * \brief Parse command line options.
 * 
 * \return True if command line option is recognized, false otherwise
 */
bool State::parseCommandLine(int argc, char** argv)
{
	// read all arguments
	std::vector< std::string > arg;
	for(int i = 0; i < argc; i++)
		arg.push_back(argv[i]);

	bCommandLine_ = true;

	// GUI related arguments
	if(arg[1] == "-gui") {
		if(argc > 3 && arg[2] == "-pardump")
			dumpParameters(arg[3], false);
	}

	// dump all parameters
	else if(arg[1] == "-pardump") {
		if(argc < 3) {
			std::cout << "No output file given for parameter dump! (usage: <executable> -pardump <filename>)" << std::endl;
		}
		else {
			std::cout << "Exporting complete parameter list to \'" << arg[2] << "\'...\n";
			dumpParameters(arg[2]);
		}
	}

	// display cmd arguments
	else if(arg[1].substr(0,2) == "-h" || arg[1].substr(0,3) == "--h") {
		std::cout << "Current command line arguments:\n";
		std::cout << "\t<parameter_file>   run ECF with given parameter file\n";
		std::cout << "\t-pardump <file>    dump all parameters in a given file\n";
		std::cout << "\t-h, -help          display this help\n";
	}

	else
		bCommandLine_ = false;

	return bCommandLine_;
}



/**
 * \brief	Is the algorithm executed implicitly parallel
 */
bool State::isImplicitParallel()
{	return algorithm_->isImplicitParallel();	}


/**
 * \brief	Is current algorithm parallel
 */
bool State::isAlgorithmParallel()
{	return algorithm_->isParallel();	}


/**
 * \brief	Save State object data to milestone.
 * Called by: State::saveMilestone
 */
void State::write(XMLNode& xState)
{
	registry_->modifyEntry("milestone.generation_", (voidP) (new uint(getGenerationNo())));
	registry_->modifyEntry("milestone.elapsedtime_", (voidP) (new time_t(elapsedTime_)));
	registry_->modifyEntry("batch.remaining_", (voidP) (new uint(batchRemaining_)));
	if(batchLogFile_ != "")
		registry_->modifyEntry("batch.logfile_", (voidP) (new std::string(batchLogFile_)));
	else
		registry_->modifyEntry("batch.logfile_", (voidP) (new std::string("*")));
}


/**
 * \brief	Save milestone to file.
 * Called by: State::run
 */
void State::saveMilestone()
{
	XMLNode xMainNode = XMLNode::createXMLTopNode("ECF");
	xMainNode.addAttribute("milestone", ctime(&currentTime_));

	XMLNode xMilestone;
	this->write(xMilestone);
	xMainNode.addChild(xMilestone);

	XMLNode xNode = this->xConfig_.getChildNode(NODE_ALGORITHM);
	xNode = xNode.deepCopy();
	xMainNode.addChild(xNode);
	xNode = this->xConfig_.getChildNode(NODE_GENOTYPE);
	xNode = xNode.deepCopy();
	xMainNode.addChild(xNode);

	this->registry_->write(xNode);
	xMainNode.addChild(xNode);

	// save population
	XMLNode xPopulation;
	population_->write(xPopulation);
	xMainNode.addChild(xPopulation);

#ifdef _MPI
	if(comm_->getCommGlobalRank() != 0)
		return;
#endif

	xMainNode.writeToFile(milestoneFilename_.c_str());
}


/**
 * \brief	Load milestone data (population and context).
 * Called by: State::initialize
 */
void State::loadMilestone()
{
	ECF_LOG(this, 4, "Loading population and evolutionary context from milestone...");
	XMLNode xPopulation = xConfig_.getChildNode("Population");
	population_->read(xPopulation);

	context_->generationNo_ = milestoneGeneration_;
}



//
// setting components
//

/**
 * \brief Set a genotype to be used in individuals.
 *
 * This actually adds new gentoype to be used in individuals (doesn't replace the ones already defined as active). 
 * Each genotype is given a unique index.
 *
 * \param genotype	new genotype to be included in each individual
 * \return	new genotype's index
 */
uint State::setGenotype(GenotypeP genotype)
{
	genotype_.push_back((GenotypeP) genotype->copy());
	uint index = (uint) genotype_.size() - 1;
	genotype_[index]->setGenotypeId(index);
	genotype->setGenotypeId(index);

	genotype_[index]->registerParameters(state_);

	// read genotype's operators and register their parameters
	crossover_->operators.push_back(genotype_[index]->getCrossoverOp());
	for(uint iOp = 0; iOp < crossover_->operators[index].size(); iOp++) {
		crossover_->operators[index][iOp]->myGenotype_ = genotype_[index];
		crossover_->operators[index][iOp]->registerParameters(state_);
	}

	mutation_->operators.push_back(genotype_[index]->getMutationOp());
	for(uint iOp = 0; iOp < mutation_->operators[index].size(); iOp++) {
		mutation_->operators[index][iOp]->myGenotype_ = genotype_[index];
		mutation_->operators[index][iOp]->registerParameters(state_);
	}

	bGenotypeSet_ = true;
	return index;
}

/// Set the desired algorithm (overrides the current choice)
void State::setAlgorithm(AlgorithmP algorithm)
{
	algorithm_ = algorithm;
	bAlgorithmSet_ = true;
}


/**
 * \brief Set user defined evaluation operator.
 *
 * Evaluation operator must be created and set before the call to State::initialize
 */
void State::setEvalOp(EvaluateOpP eval)
{
	evalOp_ = eval;
	bEvaluatorSet_ = true;
}


/**
 * \brief Set user defined evaluation operator (without a smart pointer).
 *
 * Evaluation operator must be created and set before the call to State::initialize
 */
void State::setEvalOp(EvaluateOp* eval)
{
	evalOp_ = (EvaluateOpP) eval;
	bEvaluatorSet_ = true;
}


//
// adding components
//

/**
 * \brief	Add user-defined or user customized genotype.
 * (The genotype can then be specified and used in config file.)
 * Replaces the existing genotype with the same name (if there is one).
 * Must be called before State::initialize
 *
 * \param gen	new genotype
 */
bool State::addGenotype(GenotypeP gen)
{
	mGenotypes_[gen->getName()] = gen;
	return true;
}


/**
 * \brief	Add user-defined or user customized algorithm. 
 * (It can then be specified and used in config file.) 
 * Replaces the existing algorithm with the same name (if there is one). 
 * Must be called before State::initialize
 *
 * \param gen	new algorithm
 */
bool State::addAlgorithm(AlgorithmP alg)
{
	mAlgorithms_[alg->getName()] = alg;
	return true;
}

/**
 * \brief	Add user-defined operator. 
 * (Its parameters can now be specified and used in config file.) 
 * Must be called before State::initialize
 *
 * \param gen	new operator
 */
bool State::addOperator(OperatorP op)
{
	allUserOps_.push_back(op);
	return true;
}



//
// run methods
//

#ifndef _MPI
/**
 * \brief Driver of the evolution process - serial version.
 *
 * Runs the designated algorithm.
 * Called by: ::main()
 */
bool State::run()
{
	// command line only (no evolution)
	if(bCommandLine_)
		return false;

	if(!bInitialized_) {
		std::cout << "Error: Initialization failed!" << std::endl;
		return false;
	}

	if(bBatchStart_) {
		ECF_LOG(this, 5, "Batch mode detected: running batch");
		runBatch();
		return true;
	}

	startTime_ = time(NULL);
	std::string stime = ctime(&startTime_);
	ECF_LOG(this, 3, "Start time: " + stime);
	// adjust with previous runtime (from milestone)
	startTime_ -= milestoneElapsedTime_;

	// evaluate initial population
	ECF_LOG(this, 2, "Evaluating initial population...");
	algorithm_->initializePopulation(state_);

	currentTime_ = time(NULL);
	elapsedTime_ = currentTime_ - startTime_;
	ECF_LOG(this, 2, "Generation: " + uint2str(context_->generationNo_));
	ECF_LOG(this, 2, "Elapsed time: " + uint2str((uint) elapsedTime_));
	population_->updateDemeStats();

	// call user-defined operators
	ECF_LOG(this, 4, "Calling user defined operators...");
	for(uint i = 0; i < activeUserOps_.size(); i++)
		activeUserOps_[i]->operate(state_);

	// termination ops
	ECF_LOG(this, 4, "Checking termination conditions...");
	for(uint i = 0; i < activeTerminationOps_.size(); i++)
		activeTerminationOps_[i]->operate(state_);

	// run the algorithm
	while(context_->bTerminate_ == false) {
		context_->generationNo_++;
		ECF_LOG(this, 5, "Calling the active algorithm");
		algorithm_->advanceGeneration(state_);

		currentTime_ = time(NULL);
		elapsedTime_ = currentTime_ - startTime_;
		ECF_LOG(this, 2, "Generation: " + uint2str(context_->generationNo_));
		ECF_LOG(this, 2, "Elapsed time: " + uint2str((uint) elapsedTime_));

		population_->updateDemeStats();

		// call user-defined operators
		ECF_LOG(this, 4, "Calling user defined operators...");
		for(uint i = 0; i < activeUserOps_.size(); i++)
			activeUserOps_[i]->operate(state_);

		// termination ops
		ECF_LOG(this, 4, "Checking termination conditions...");
		for(uint i = 0; i < activeTerminationOps_.size(); i++)
			activeTerminationOps_[i]->operate(state_);

		if(context_->bTerminate_)
			logger_->saveTo(true);
		else
			logger_->saveTo();

		if(bSaveMilestone_ && 
				milestoneInterval_ > 0 && context_->generationNo_ % milestoneInterval_ == 0)
			saveMilestone();

		migration_->operate(state_);
	}

	// output HallOfFame
	XMLNode xHoF;
	population_->getHof()->write(xHoF);
	char *out = xHoF.createXMLString(true);
	ECF_LOG(this, 1, "\nBest of run: \n" + std::string(out));
	freeXMLString(out);

	logger_->saveTo(true);
	if(bSaveMilestone_)
		saveMilestone();

	logger_->closeLog();

	return true;
}


#else // _MPI
/**
 * \brief Driver of the evolution process - parallel version.
 *
 * Runs the designated algorithm.
 * Called by: ::main()
 */
bool State::run()
{
	// command line only (no evolution)
	if(bCommandLine_)
		return false;

	if(bBatchStart_) {
		runBatch();
		return true;
	}

	// TODO: perform AND_reduce with bInitialized on all processes
	if(!bInitialized_) {
		std::cout << "Error: Initialization failed!" << std::endl;
		logger_->saveTo();
		if(comm_->isInitialized())
			comm_->finalize();
		return false;
	}

	startTime_ = time(NULL);
	std::string stime = ctime(&startTime_);
	ECF_LOG(this, 3, "Start time: " + stime);
	// adjust with previous runtime (from milestone)
	startTime_ -= milestoneElapsedTime_;

	// in PECF, every process works with deme '0'
	// 'deme masters' - processes with index 0 in local communicator
	// only deme masters know of whole population
	if(comm_->getCommRank() == 0) {
		ECF_LOG(this, 2, "Evaluating initial population...");
	}

	// every process participates in the initial population evaluation
	algorithm_->initializePopulation(state_);
	comm_->synchronize();

	if(comm_->getCommRank() == 0) {
		// dodatna inicijalizacija za implicitni paralelizam - mozda promijeniti...
		if(isImplicitParallel())
			algorithm_->initializeImplicit(state_);
	}

	// run the algorithm
	while(context_->bTerminate_ == false) {

		currentTime_ = time(NULL);
		elapsedTime_ = currentTime_ - startTime_;
		if(comm_->getCommGlobalRank() == 0) {
			ECF_LOG(this, 2, "Generation: " + uint2str(context_->generationNo_));
			ECF_LOG(this, 2, "Elapsed time: " + uint2str((uint) elapsedTime_));
		}	

		// deme masters initiate population statistics and HoF update
		if(comm_->getCommRank() == 0) {
			population_->updateDemeStats();

			if(bSaveMilestone_ && milestoneInterval_ > 0 && context_->generationNo_ % milestoneInterval_ == 0)
				saveMilestone();
		}

		// global process 0 checks termination condition and signals deme masters
		if(comm_->getCommGlobalRank() == 0) {
			ECF_LOG(this, 4, "Checking termination conditions...");
			for(uint i = 0; i < activeTerminationOps_.size(); i++)
				activeTerminationOps_[i]->operate(state_);

			for(uint i = 1; i < population_->getNoDemes(); i++)
				comm_->sendTerminateMessage(comm_->getDemeMaster(i), context_->bTerminate_);
		}
		// deme masters receive info
		else if(comm_->getCommRank() == 0)
			context_->bTerminate_ = comm_->recvTerminateMessage(0);

		// deme masters signal workers of evolution continuation
		algorithm_->bcastTermination(state_);

		// deme masters call migration operator
		if(comm_->getCommRank() == 0) {
			migration_->operate(state_);
		}

		// call user-defined operators
		ECF_LOG(this, 4, "Calling user defined operators...");
		for(uint i = 0; i < activeUserOps_.size(); i++)
			activeUserOps_[i]->operate(state_);

		if(context_->bTerminate_ == true) {
			logger_->saveTo(true);
			break;
		}

		logger_->saveTo();

		context_->generationNo_++;
		ECF_LOG(this, 5, "Calling the active algorithm");
		algorithm_->advanceGeneration(state_);
	}

	logger_->setLogFrequency(1);
	if(comm_->getCommGlobalRank() == 0) {
		// output HallOfFame
		XMLNode xHoF;
		population_->getHof()->write(xHoF);
		std::string out = xHoF.createXMLString(true);
		ECF_LOG(this, 1, "\nBest of run: \n" + out);

		logger_->saveTo(true);
	}

	if(comm_->getCommRank() == 0 && bSaveMilestone_)
		saveMilestone();

	logger_->saveTo(true);

	// communicator
	comm_->finalize();

	return true;
}
#endif // _MPI