#include "ECF_base.h"

const int RANDOM_GENOTYPE = 0;
const int ALL_GENOTYPES = 1;


/**
 * \brief Return current individual mutation probability
 */
double Mutation::getIndMutProb()
{
	return indMutProb_;
}


/**
 * \brief Set new individual mutation probability (ignored if not [0, 1])
 */
double Mutation::setIndMutProb(double newProb)
{
	if(newProb >= 0 && newProb <= 1)
		indMutProb_ = newProb;
	return indMutProb_;
}


/**
 * \brief Register mutation related but Genotype unrelated parameters
 */
void Mutation::registerParameters(StateP state)
{
	state->getRegistry()->registerEntry("mutation.indprob", (voidP) new double(0.3), ECF::DOUBLE,
		"individual mutation probability (unless the algorithm overrides it) (default: 0.3)");
//	state->getRegistry()->registerEntry("mutation.geneprob", (voidP) new double(0.01), ECF::DOUBLE);
	state->getRegistry()->registerEntry("mutation.genotypes", (voidP) new std::string("random"), ECF::STRING,
		"if there are multiple genotypes, which to mutate? 'random': a random one, all: mutate all (default: random)");
	state->getRegistry()->registerEntry("mutation.protected", (voidP) new std::string(""), ECF::STRING,
		"indexes of genotypes (separated by spaces) that are excluded (protected) from mutation (default: none)");
}


/**
 * \brief Initialize all mutation operators of all active genotypes
 */
bool Mutation::initialize(StateP state)
{
	state_ = state;
	protectedGenotypes_.clear();
	protectedGenotypes_.insert(protectedGenotypes_.begin(), operators.size(), false);
	opProb.clear();

	voidP sptr = state->getRegistry()->getEntry("mutation.indprob");
	indMutProb_ = *((double*)sptr.get());

	sptr = state->getRegistry()->getEntry("mutation.geneprob");
//	geneMutProb_ = *((double*)sptr.get());
//	if(state->getRegistry()->isModified("mutation.geneprob") == false)
//		geneMutProb_ = 0;

	sptr = state->getRegistry()->getEntry("mutation.genotypes");
	std::string mutGen = *((std::string*)sptr.get());

	mutateGenotypes_ = RANDOM_GENOTYPE;
	if(mutGen == "random")
		mutateGenotypes_ = RANDOM_GENOTYPE;
	else if(mutGen == "all")
		mutateGenotypes_ = ALL_GENOTYPES;
	else
		ECF_LOG_ERROR(state, "Warning: invalid parameter value (key: mutation.genotypes)");

	// read protected genotypes
	std::stringstream ss;
	sptr = state->getRegistry()->getEntry("mutation.protected");
	ss << *((std::string*) sptr.get());
	uint genId;
	while(ss >> genId) {	// read all the data from string
		if(genId >= protectedGenotypes_.size()) {
			ECF_LOG_ERROR(state, "Error: invalid genotype index (key: mutation.protected)!");
			throw("");
		}
		protectedGenotypes_[genId] = true;
	}

	// initialize operators for all genotypes
	for(uint gen = 0; gen < operators.size(); gen++) {
		uint nOps = (uint) operators[gen].size();
		// if the genotype doesn't define mutation operators
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
		if(probs[nOps - 1] == 0) {
			std::vector<double> none(1);
			none[0] = -1;
			opProb.push_back(none);
		} else {
			if(probs[nOps - 1] != 1) {
				double normal = probs[nOps - 1];
				ECF_LOG_ERROR(state, "Warning: " + operators[gen][0]->myGenotype_->getName() +
					" mutation operators: cumulative probability not equal to 1 (sum = " + dbl2str(normal) + ")");
				for(uint i = 0; i < probs.size(); i++)
					probs[i] /= normal;
			}
			opProb.push_back(probs);
		}
	}
	return true;
}


/**
 * \brief Perform mutation on a pool of individuals.
 *
 * Mutation probability may be defined with 2 parameters:
 * - (not implemented!) if geneMutProb_ is defined in config, use that parameter with genotype's genome size 
 * - otherwise use indMutProb_
 * 
 * Called by: Algorithm::advanceGeneration
 * \return	number of mutated individuals
 */
uint Mutation::mutation(const std::vector<IndividualP> &pool)
{
	uint mutated = 0;
	for(uint i = 0; i < pool.size(); i++) {
		if(state_->getRandomizer()->getRandomDouble() <= indMutProb_) {
			mutated++;
			mutate(pool[i]);
		}
	}

	return mutated;
}


/**
 * \brief Mutate an individual.
 *
 * May mutate one or more genotypes in given individual.
 * Determines which MutationOp to use on each genotype.
 */
bool Mutation::mutate(IndividualP ind)
{	
	ind->fitness->setInvalid();
	// set mutation context
	state_->getContext()->mutatedIndividual = ind;
	ECF_LOG(state_, 5, "Mutating individual: " + ind->toString());
	currentInd = ind;

	// if mutating a random genotype
	if(mutateGenotypes_ == RANDOM_GENOTYPE) {
		uint iGenotype = state_->getRandomizer()->getRandomInteger((int)ind->size());
		if(protectedGenotypes_[iGenotype])
			return false;
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
		operators[iGenotype][iOperator]->mutate(ind->at(iGenotype));
	}

	// if mutating all genotypes in the individual
	else if(mutateGenotypes_ == ALL_GENOTYPES) {
		for(uint iGenotype = 0; iGenotype < ind->size(); iGenotype++) {
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
			operators[iGenotype][0]->mutate(ind->at(iGenotype));
		}
	}

	ECF_LOG(state_, 5, "Mutated individual: " + ind->toString());

	return true;
}

