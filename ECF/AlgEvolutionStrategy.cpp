#include "ECF_base.h"
#include "ECF_macro.h"
#include "AlgEvolutionStrategy.h"
#include "SelBestOp.h"
#include "SelRandomOp.h"
#include <algorithm>


EvolutionStrategy::EvolutionStrategy()
{
	// define algorithm name
	name_ = "EvolutionStrategy";

	// create selection operators needed
	selBestOp_ = static_cast<SelectionOperatorP> (new SelBestOp);
	selRandomOp_ = static_cast<SelectionOperatorP> (new SelRandomOp);
}


void EvolutionStrategy::registerParameters(StateP state)
{
	registerParameter(state, "lambda", (voidP) new uint(4), ECF::UINT, 
		"number of offspring created in each iteration (default: 4)");
	registerParameter(state, "rho", (voidP) new uint(1), ECF::UINT, 
		"number of parents used to create an offspring; may be 1 or 2 (default: 1)");
	registerParameter(state, "mu", (voidP) new uint(1), ECF::UINT, 
		"the size of parent population (default: 1)");
	registerParameter(state, "selection", (voidP) new std::string("plus"), ECF::STRING, 
		"selection scheme: \"plus\", uses both parents and offspring) or \"comma\", uses just offspring (default: plus)");
}


bool EvolutionStrategy::initialize(StateP state)
{
	// initialize all operators
	selBestOp_->initialize(state);
	selRandomOp_->initialize(state);

	// read parameter values
	voidP sizep = getParameterValue(state, "lambda");
	lambda_ = *((uint*) sizep.get());

	voidP rhop = getParameterValue(state, "rho");
	rho_ = *((uint*) rhop.get());

	if(rho_ < 1 || rho_ > 2) {
		ECF_LOG_ERROR(state, "Error: number of parents to create an offspring in EvolutionStrategy can only be 1 or 2!");
		throw "";
	}

	voidP mup = getParameterValue(state, "mu");
	mu_ = *((uint*) mup.get());

	if(mu_ < rho_) {
		ECF_LOG_ERROR(state, "Error: size of parent population in EvolutionStrategy must be greater than number of parents to create an offspring!");
		throw "";
	}


	voidP sptr = state->getRegistry()->getEntry("population.size");
	uint demeSize = *((uint*) sptr.get());
	if(demeSize % mu_ != 0 || mu_ < 1) {
		ECF_LOG_ERROR(state, "Error: \"population.size\" parameter must be a multiple of size of parent population (mu) in EvolutionStrategy algorithm!");
		throw "";
	}

	voidP selp = getParameterValue(state, "selection");
	std::string sels = *((std::string*) selp.get());
	if(sels == "plus")
		plusSelection_ = true;
	else if(sels == "comma")
		plusSelection_ = false;
	else {
		ECF_LOG_ERROR(state, "Error: selection type in EvolutionStrategy can only be \"plus\" or \"comma\"!");
		throw "";
	}

	if(plusSelection_ == false && lambda_ <= mu_) {
		ECF_LOG_ERROR(state, "Error: offspring number (lambda) in comma EvolutionStrategy must be greater than the number of parents (mu)!");
		throw "";
	}

	return true;
}


bool EvolutionStrategy::advanceGeneration(StateP state, DemeP deme)
{
	subPopulations_ = (uint) deme->size() / mu_;

	// repeat the same ES for each parent subpopulation
	for(uint subPopulation = 0; subPopulation < subPopulations_; subPopulation++) {

		// use appropriate portion of the deme
		uint firstInd = subPopulation * mu_;
		uint lastInd = (subPopulation + 1) * mu_;

		// construct parent pool
		std::vector<IndividualP> parents;
		for(uint ind = firstInd; ind < lastInd; ind++)
			parents.push_back(deme->at(ind));

		// create offspring pool
		std::vector<IndividualP> offspring;
		for(uint iChild = 0; iChild < lambda_; iChild++) {
			// randomly select rho parents, create one child
			IndividualP child;
			// use mutation
			if(rho_ == 1) {
				IndividualP parent = selRandomOp_->select(parents);
				child = copy(parent);
				mutation_->mutate(child);
			}
			// use crossover
			else if(rho_ == 2) {
				IndividualP p1 = selRandomOp_->select(parents);
				IndividualP p2 = p1;
				while(p1 == p2)
					p2 = selRandomOp_->select(parents);
				child = copy(p1);
				mate(p1, p2, child);
			}
			offspring.push_back(child);
			// evaluate offspring
			evaluate(child);
		}

		// construct selection pool for new generation parents
		std::vector<IndividualP> selPool;
		if(plusSelection_) {
			// arrange selection pool from both best parents and offspring
			selPool = parents;
			selPool.insert(selPool.end(), offspring.begin(), offspring.end());
		}
		else
			// select from offspring only
			selPool = offspring;

		// sort by fitness (best first)
		std::sort(selPool.begin(), selPool.end(), &EvolutionStrategy::compare);

		// replace new generation in deme
		uint selected = 0;
		for(uint ind = firstInd; ind < lastInd; ind++, selected++)
			replaceWith(ind, selPool[selected]);
	}

	return true;
}
