#include "ECF_base.h"
#include "ECF_macro.h"
#include "SelRandomOp.h"
#include "AlgDifferentialEvolution.h"
#include "floatingpoint/FloatingPoint.h"
#include <iostream>


DifferentialEvolution::DifferentialEvolution()
{
	// define algorithm name
	name_ = "DifferentialEvolution";

	// by default, the algorithm is unconstrained
	bounded_ = false;

	// create selection operators needed
	selRandomOp = static_cast<SelectionOperatorP> (new SelRandomOp);
}


void DifferentialEvolution::registerParameters(StateP state)
{
	registerParameter(state, "F", (voidP) new double(1.), ECF::DOUBLE, "scaling constant");
	registerParameter(state, "CR", (voidP) new double(0.9), ECF::DOUBLE, "crossover rate");
	registerParameter(state, "bounded", (voidP) new uint(0), ECF::UINT, 
		"should the algorithm respect the bounds defined in the genotype or not (default: 0)");
}


bool DifferentialEvolution::initialize(StateP state)
{	
	selRandomOp->initialize(state);
	donor_vector.clear();

	// read parameters, check defined genotype (only a single FloatingPoint is allowed)
	voidP F = getParameterValue(state, "F");
	Fconst_ = *((double*) F.get());
	voidP CR = getParameterValue(state, "CR");
	CR_ = *((double*) CR.get());

	// algorithm accepts a single FloatingPoint or Binary genotype 
	// or a genotype derived from the abstract RealValueGenotype class
	GenotypeP activeGenotype = state->getGenotypes()[0];
	RealValueGenotypeP rv = boost::dynamic_pointer_cast<RealValueGenotype> (activeGenotype);
	if(!rv) {
		ECF_LOG_ERROR(state, "Error: Differential evolution algorithm accepts only a RealValueGenotype derived genotype! (FloatingPoint or Binary)");
		throw ("");
	}

	voidP bounded = getParameterValue(state, "bounded");
	bounded_ = *((bool*) bounded.get());

	return true;
}


bool DifferentialEvolution::advanceGeneration(StateP state, DemeP deme)
{
	// create donor vectors for each population member
	for(uint iIter = 0; iIter < deme->size(); iIter++){ 
		createDonorVectors(deme, state);
	}

	// perform DE crossover, generate trial vectors (stored in donor_vector)
	for(uint iIter = 0; iIter < deme->size(); iIter++) {
		crossover(deme, iIter, state);
	}
	
	// select the better one for each population member and trial vector
	for(uint iIter = 0; iIter < deme->size(); iIter++) {
		evaluate(donor_vector[iIter]);
		if(donor_vector[iIter]->fitness->isBetterThan(deme->at(iIter)->fitness))
			replaceWith(deme->at(iIter), donor_vector[iIter]);
	}

	//for(uint i = 0; i < deme->size(); i++){
	//	state->getLogger()->log(5, "deme[" + uint2str(i) + "]: " + dbl2str(deme->at(i)->fitness->getValue()) + "\t" + uint2str(deme->at(i)->index));
	//}

	donor_vector.clear();

	return true;
}


//! create donor vectors (the same number as the population size)
void DifferentialEvolution::createDonorVectors(DemeP deme, StateP state)
{
	IndividualP ind1, ind2, ind3;

	// select three different population members
	ind1 = selRandomOp->select(*deme);
	ind2 = selRandomOp->select(*deme);
	while (ind1->index == ind2->index)
		ind2 = selRandomOp->select(*deme);
	ind3 = selRandomOp->select(*deme); 	
	while (ind1->index == ind3->index || ind2->index == ind3->index)
		ind3 = selRandomOp->select(*deme);

	// get their genotypes
	RealValueGenotype* flp1 = (RealValueGenotype*) (ind1->getGenotype().get());
	RealValueGenotype* flp2 = (RealValueGenotype*) (ind2->getGenotype().get());
	RealValueGenotype* flp3 = (RealValueGenotype*) (ind3->getGenotype().get());

	// create new individual to contain new donor vector
	IndividualP v (new Individual(state));
	RealValueGenotype* b = (RealValueGenotype*) v->getGenotype().get();
	double donor_value;
	
	// calculate new donor vector elements
	for(uint i = 0; i < flp2->realValue.size(); i++){
			donor_value = flp1->realValue[i] + Fconst_ * (flp2->realValue[i] - flp3->realValue[i]);

			// check for bounds
			if(bounded_) {
				if(donor_value < b->getLBound())
					donor_value = b->getLBound();
				if(donor_value > b->getUBound())
					donor_value = b->getUBound();
			}

			b->realValue[i] = donor_value;
	}

	donor_vector.push_back(v);
}


//! cross donor vectors with population members to create trial vectors
void DifferentialEvolution::crossover(DemeP deme, uint index, StateP state)
{
	// get population member and corresponding donor vector
	RealValueGenotype* flp1 = (RealValueGenotype*) (deme->at(index)->getGenotype().get());
	int dim = (int) flp1->realValue.size();
	RealValueGenotype* flp2 = (RealValueGenotype*) donor_vector[index]->getGenotype().get();

	// crossover their elements (keep the result in donor_vector)
	for(uint i = 0; i < flp1->realValue.size(); i++) {
		if (state->getRandomizer()->getRandomDouble() <= CR_ || i == state->getRandomizer()->getRandomInteger(dim)) {
		}
		else {
			flp2->realValue[i] = flp1->realValue[i];
	    }
	}

}