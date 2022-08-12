#include "ECF_base.h"

const int RANDOM_GENOTYPE = 0;
const int ALL_GENOTYPES = 1;


/**
 * \brief Register crossover related but Genotype unrelated parameters
 */
void Crossover::registerParameters(StateP state)
{	
	state->getRegistry()->registerEntry("crossover.genotypes", (voidP) new std::string("random"), ECF::STRING,
		"if there are multiple genotypes, which to cross? 'random': a random pair, 'all': all pairs (default: random)");
	state->getRegistry()->registerEntry("crossover.protected", (voidP) new std::string(""), ECF::STRING,
		"indexes of genotypes (separated by spaces) that are excluded (protected) from crossover (default: none)");
}


/**
 * \brief Initialize all crossover operators of all active genotypes
 */
bool Crossover::initialize(StateP state)
{
	state_ = state;
	protectedGenotypes_.clear();
	protectedGenotypes_.insert(protectedGenotypes_.begin(), operators.size(), false);
	opProb.clear();

	voidP sptr = state->getRegistry()->getEntry("crossover.genotypes");
	std::string crxGen = *((std::string*)sptr.get());

	crxGenotypes_ = RANDOM_GENOTYPE;
	if(crxGen == "all")
		crxGenotypes_ = ALL_GENOTYPES;
	else if(crxGen == "random")
		crxGenotypes_ = RANDOM_GENOTYPE;
	else
		ECF_LOG_ERROR(state, "Warning: invalid parameter value (key: crossover.genotypes)");

	// read protected genotypes
	std::stringstream ss;
	sptr = state->getRegistry()->getEntry("crossover.protected");
	ss << *((std::string*) sptr.get());
	uint genId;
	while(ss >> genId) {	// read all the data from string
		if(genId >= protectedGenotypes_.size()) {
			ECF_LOG_ERROR(state, "Error: invalid genotype index (key: crossover.protected)!");
			throw("");
		}
		protectedGenotypes_[genId] = true;
	}

	// initialize operators for all genotypes
	for(uint gen = 0; gen < operators.size(); gen++) {
		uint nOps = (uint) operators[gen].size();
		// if the genotype doesn't define crossover operators
		if(nOps == 0) {
			protectedGenotypes_[gen] = true;
			std::vector<double> empty;
			opProb.push_back(empty);
			break;
		}
		for(uint i = 0; i < nOps; i++) {
			operators[gen][i]->state_ = state;
			operators[gen][i]->initialize(state);
		}
		// calculate cumulative operator probabilities
		std::vector<double> probs(nOps);
		probs[0] = operators[gen][0]->probability_;
		for(uint i = 1; i < nOps; i++) {
			probs[i] = probs[i - 1] + operators[gen][i]->probability_;
		}
		if(probs[nOps - 1] == 0)
			probs[0] = -1;
		else
			if(probs[nOps - 1] != 1) {
				double normal = probs[nOps - 1];
				ECF_LOG_ERROR(state, "Warning: " + operators[gen][0]->myGenotype_->getName() +
					" crossover operators: cumulative probability not equal to 1 (sum = " + dbl2str(normal) + ")");
				for(uint i = 0; i < probs.size(); i++)
					probs[i] /= normal;
			}
		opProb.push_back(probs);
	}
	return true;
}


/**
 * \brief Crosses 2 individuals.
 *
 * If any of the parent individuals is also the child, makes a copy of that individual.
 * Determines which CrossoverOp will be used to cross each genotype.
 */
bool Crossover::mate(IndividualP ind1, IndividualP ind2, IndividualP child)
{
	child->fitness->setInvalid();
	// set crossover context
	state_->getContext()->firstParent = ind1;
	state_->getContext()->secondParent = ind2;
	state_->getContext()->child = child;
	ECF_LOG(state_, 5, "Crossover, 1st parent: " + ind1->toString() 
		+ "\nCrossover, 2nd parent: " + ind2->toString());

	// check if any of the parents is also the child
	if(ind1 == child) {
		ind1 = (IndividualP) ind1->copy();
	}
	if(ind2 == child) {
		ind2 = (IndividualP) ind2->copy();
	}

	// TODO: parameter for genotype exchange (instead of genotype crossover)

	// default: choose radnom genotype pair to crx
	if(crxGenotypes_ == RANDOM_GENOTYPE) {
		uint iGenotype = state_->getRandomizer()->getRandomInteger((int)ind1->size());
		if(protectedGenotypes_[iGenotype])
			return false;
		// copy unchanged genotypes from parents
		for(uint i = 0; i < iGenotype; i++)
			child->at(i) = (GenotypeP) ind1->at(i)->copy();
		for(uint i = iGenotype + 1; i < child->size(); i++)
			child->at(i) = (GenotypeP) ind2->at(i)->copy();
		// choose operator
		uint iOperator;
		if(opProb[iGenotype][0] < 0)
			iOperator = state_->getRandomizer()->getRandomInteger((int)operators[iGenotype].size());
		else {
			double random = state_->getRandomizer()->getRandomDouble();
			iOperator = 0;
			while(opProb[iGenotype][iOperator] < random)
				iOperator++;
		}
		operators[iGenotype][iOperator]->mate(ind1->at(iGenotype), ind2->at(iGenotype), child->at(iGenotype));
	}
	// otherwise: crx all genotypes
	else if(crxGenotypes_ == ALL_GENOTYPES) {
		for(uint iGenotype = 0; iGenotype < ind1->size(); iGenotype++) {
			if(protectedGenotypes_[iGenotype])
				continue;
			// choose operator
			uint iOperator;
			if(opProb[iGenotype][0] < 0)
				iOperator = state_->getRandomizer()->getRandomInteger((int)operators[iGenotype].size());
			else {
				double random = state_->getRandomizer()->getRandomDouble();
				iOperator = 0;
				while(opProb[iGenotype][iOperator] < random)
					iOperator++;
			}
			operators[iGenotype][iOperator]->mate(ind1->at(iGenotype), ind2->at(iGenotype), child->at(iGenotype));
		}
	}

	ECF_LOG(state_, 5, "Crossover, new individual: " + child->toString());

	return true;
}
