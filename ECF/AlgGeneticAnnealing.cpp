#include "ECF_base.h"
#include "ECF_macro.h"
#include "AlgGeneticAnnealing.h"


GeneticAnnealing::GeneticAnnealing()
{
	// define algorithm name
	name_ = "GeneticAnnealing";

	selBestOp_ = static_cast<SelBestOpP> (new SelBestOp);
}


void GeneticAnnealing::registerParameters(StateP state)
{
	registerParameter(state, "energybank", (voidP) new double(200), ECF::DOUBLE,
		"total starting energy bank (fitness dependant)");
	registerParameter(state, "coolingfactor", (voidP) new double (0.7), ECF::DOUBLE,
		"simulated annealing cooling factor"); 
	registerParameter(state, "elitism", (voidP) new uint (0), ECF::UINT,
		"is the best individual preserved"); 
}


bool GeneticAnnealing::initialize(StateP state)
{
	// read parameter values 
	voidP p = getParameterValue(state, "energybank");
	this->energyBank_ = *((double*) p.get());

	if(energyBank_ <= 0) {
		ECF_LOG(state, 1, "Error: GeneticAnnealing algorithm parameter 'energybank' must be greater than zero!");
		throw "";
	}

	p = getParameterValue(state, "coolingfactor");
	this->coolingF_ = *((double*) p.get());

	if ((coolingF_ < 0)||(coolingF_>=1)){
		ECF_LOG(state, 1, "Error: GeneticAnnealing algorithm parameter 'coolingfactor' must be in <0, 1]!");
		throw "";
	}

	p = getParameterValue(state, "elitism");
	elitism_ = *((uint*) p.get()) ? true : false;

	isFirstGeneration_ = true;

	selBestOp_->initialize(state);

    return true;
}


bool GeneticAnnealing::advanceGeneration(StateP state, DemeP deme)
{
	uint N = (uint) deme->size();
	uint i;
	IndividualP mutant, ind;

	IndividualP best = selBestOp_->select(*deme);

	if (isFirstGeneration_){
		Th = (double *) malloc (N*sizeof(double));
		for (i = 0; i < N; i++){
			ind = deme->at(i);
			Th[i] = ind->fitness->getValue() + energyBank_/N;
		} // init of threshold

		energyBank_ = 0;
		isFirstGeneration_ = false;
	}

	for (i = 0; i < N; i++){
		ind = deme->at(i);

		if(elitism_ && ind == best)
			continue;

		mutant = (IndividualP) ind->copy();
		mutate (mutant); // create mutant
		evaluate(mutant);

		double eMutant = mutant->fitness->getValue();
		
		if (eMutant <= Th[i]){
			energyBank_ = energyBank_ + Th[i] - eMutant;
			Th[i] = eMutant;
			replaceWith(ind, mutant); // replace individual with mutant
		}
	}

	dE_ = energyBank_ * coolingF_ / N;
	ECF_LOG(state, 4, "GeneticAnnealing: energy bank=" + dbl2str(energyBank_) + ", dE_=" + dbl2str(dE_));

	energyBank_ = 0;
	for (i = 0; i < N; i++)
		Th[i] += dE_;

	return true;
}

 			
	