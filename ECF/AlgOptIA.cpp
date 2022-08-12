#include "ECF_base.h"
#include "ECF_macro.h"
#include "AlgOptIA.h"
#include <cmath>



OptIA::OptIA()
{	// define algorithm name
	name_ = "OptIA";

	areGenotypesAdded_ = false;
}


void OptIA::registerParameters(StateP state)
{	
	registerParameter(state, "dup", (voidP) new uint(5), ECF::INT,
		"number of clones for each individual in clone population (dafault: 5)");
	registerParameter(state, "c", (voidP) new double(0.2), ECF::DOUBLE,
		"mutation rate (default: 0.2)");
	registerParameter(state, "tauB", (voidP) new double(100), ECF::DOUBLE,
		"maximum number of generations to keep an individual without improvement (default: 100)");
	registerParameter(state, "elitism", (voidP) new uint(0), ECF::UINT,
		"use elitism (default: 0)");
}


bool OptIA::initialize(StateP state)
{		
	voidP lBound = state->getGenotypes()[0]->getParameterValue(state, "lbound");
	lbound = *((double*) lBound.get());

	voidP uBound = state->getGenotypes()[0]->getParameterValue(state, "ubound");
	ubound = *((double*) uBound.get());

	voidP dimension_ = state->getGenotypes()[0]->getParameterValue(state, "dimension");
	dimension = *((uint*) dimension_.get());

	voidP dup_ = getParameterValue(state, "dup");
	dup = *((uint*) dup_.get());
	if( *((int*) dup_.get()) <= 0 ) {
		ECF_LOG(state, 1, "Error: opt-IA requires parameter 'dup' to be an integer greater than 0");
		throw "";}

	voidP c_ = getParameterValue(state, "c");
	c = *((double*) c_.get());
	if( c <= 0 ) {
		ECF_LOG(state, 1, "Error: opt-IA requires parameter 'c' to be a double greater than 0");
		throw "";}

	voidP tauB_ = getParameterValue(state, "tauB");
	tauB = *((double*) tauB_.get());
	if( tauB < 0 ) {
		ECF_LOG(state, 1, "Error: opt-IA requires parameter 'tauB' to be a nonnegative double value");
		throw "";}

	voidP elitism_ = getParameterValue(state, "elitism");
	elitism = (*((uint*) elitism_.get())) != 0;

	// algorithm accepts a single FloatingPoint or Binary genotype 
	// or a genotype derived from the abstract RealValueGenotype class
	GenotypeP activeGenotype = state->getGenotypes()[0];
	RealValueGenotypeP rv = boost::dynamic_pointer_cast<RealValueGenotype> (activeGenotype);
	if(!rv) {
		ECF_LOG_ERROR(state, "Error: opt-IA accepts only a RealValueGenotype derived genotype! (FloatingPoint or Binary)");
		throw ("");
	}

	// batch run check
	if(areGenotypesAdded_)
		return true;

	// algorithm adds another FloatingPoint genotype (age)
	FloatingPointP flpoint[2];
	for(uint iGen = 1; iGen < 2; iGen++) {
		flpoint[iGen] = (FloatingPointP) new FloatingPoint::FloatingPoint;
		state->setGenotype(flpoint[iGen]);

		flpoint[iGen]->setParameterValue(state, "dimension", (voidP) new uint(1));					

		// initial value of age parameter should be (or as close as possible to) 0				
		flpoint[iGen]->setParameterValue(state, "lbound", (voidP) new double(0));
		flpoint[iGen]->setParameterValue(state, "ubound", (voidP) new double(0.01));
		
	}
	ECF_LOG(state, 1, "opt-IA algorithm: added 1 FloatingPoint genotype (antibody age)");

	// mark adding of genotypes
	areGenotypesAdded_ = true;
	
    return true;
}


bool OptIA::advanceGeneration(StateP state, DemeP deme)
{	
	std::vector<IndividualP> clones;
	 
	cloningPhase(state, deme, clones);
	hypermutationPhase(state, deme, clones);
	agingPhase(state, deme, clones);
	selectionPhase(state, deme, clones);
    birthPhase(state, deme, clones);
	replacePopulation(state, deme, clones);

	return true;
}


bool OptIA::cloningPhase(StateP state, DemeP deme, std::vector<IndividualP> &clones)
{
	// storing all antibodies in a vector
	for( uint i = 0; i < deme->getSize(); i++ )  // for each antibody	
		clones.push_back(deme->at(i));

	for( uint i = 0; i < deme->getSize(); i++ ){ // for each antibody in clones vector
		IndividualP antibody = clones.at(i);
		
		// static cloning is fitness independent : : cloning each antibody dup times
		for (uint j = 0; j < dup; j++) 
			clones.push_back(copy(antibody));					
	}

	return true;
}


bool OptIA::hypermutationPhase(StateP state, DemeP deme, std::vector<IndividualP> &clones)
{	
	uint M;	// M - number of mutations of a single antibody 
	uint k;

	//sort 
	std::sort (clones.begin(), clones.end(), sortPopulationByFitness);

	for( uint i = 0; i < clones.size(); i++ ){ // for each antibody in vector clones
		IndividualP antibody = clones.at(i);
		
		FloatingPointP flp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (antibody->getGenotype(0));
	    std::vector< double > &antibodyVars = flp->realValue;
		
		k = 1 + i/(dup+1);
		M =(int) ((1- 1/(double)(k)) * (c*dimension) + (c*dimension));
		
		// mutate M times
		for (uint j = 0; j < M; j++){
			uint param = state->getRandomizer()->getRandomInteger((int)antibodyVars.size());
			
			double randDouble1 = state->getRandomizer()->getRandomDouble();
			double randDouble2 = state->getRandomizer()->getRandomDouble();
			double value = antibodyVars[param] + (1-2*randDouble1)* 0.2 *  (ubound - lbound) * pow(2, -16*randDouble2 );
			
			if (value > ubound)
				value = ubound;
			else if (value <lbound)
				value = lbound;

			//produce a mutation on the antibody 
			antibodyVars[param] = value;
		}
		FitnessP parentFitness = antibody->fitness;
		evaluate(antibody);

		// if the clone is better than its parent, reset clone's age
		if(antibody-> fitness->isBetterThan(parentFitness)){					
			flp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (antibody->getGenotype(1));
			double &age = flp->realValue[0];
			age = 0;
		} 
	}
	return true;
}


bool OptIA::agingPhase(StateP state, DemeP deme,  std::vector<IndividualP> &clones)
{	
	//sort 
	std::sort (clones.begin(), clones.end(), sortPopulationByFitness);

	std::vector<IndividualP> temp_clones;

	for (uint i = 0; i < clones.size(); i++){// for each antibody
		IndividualP antibody = clones.at(i);

		//age each antibody
		FloatingPointP flp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (antibody->getGenotype(1));
		double &age = flp->realValue[0];
		age += 1;
		
		// static aging: if an antibody exceeds tauB number of trials, it is replaced with a new randomly created antibody
		if (age <=tauB)
			temp_clones.push_back(antibody);
		// if elitism = true , preserve the best antibody regardless of its age
		else if (elitism == 1 && i == 0)
			temp_clones.push_back(antibody);
	}
	clones = temp_clones;
	return true;
}


bool OptIA::selectionPhase(StateP state, DemeP deme, std::vector<IndividualP> &clones)
{	
	//sort 
	std::sort (clones.begin(), clones.end(), sortPopulationByFitness);

	//keep best populationSize antibodies ( or all if the number of clones is less than that ), erase the rest
	if(clones.size() > deme->getSize())
		clones.erase (clones.begin()+ deme->getSize(), clones.end());

	return true;
}


bool OptIA::birthPhase(StateP state, DemeP deme, std::vector<IndividualP> &clones)
{
	//number of new antibodies (randomly created)
	uint birthNumber = deme->getSize() - clones.size();

	//if no new antibodies are needed, return (this if part is optional, code works fine w/o it)
	if (birthNumber == 0) return true;

	IndividualP newAntibody = copy(deme->at(0));
	FloatingPointP flp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (newAntibody->getGenotype(0));

	for (uint i = 0; i<birthNumber; i++){
		//create a random antibody
		flp->initialize(state);
		evaluate(newAntibody);
	
		//reset its age
		flp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (newAntibody->getGenotype(1));
		double &age = flp->realValue[0];
		age = 0;

		//add it to the clones vector
		clones.push_back(copy(newAntibody));
	}
	return true;
}


//! replace population with the contents of the clones vector
bool OptIA::replacePopulation(StateP state, DemeP deme, std::vector<IndividualP> &clones)
{
	for( uint i = 0; i < clones.size(); i++ ) // for each antibody
		deme->replace(i, clones.at(i));
	
	clones.clear();
	
	return true;
}
