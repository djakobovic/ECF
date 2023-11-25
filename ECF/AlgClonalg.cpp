#include "ECF_base.h"
#include "ECF_macro.h"
#include "AlgClonalg.h"
#include <cmath>
#include <algorithm>

Clonalg::Clonalg()
{
	// define algorithm name
	name_ = "Clonalg";

	areGenotypesAdded_ = false;
}


void Clonalg::registerParameters(StateP state)
{	
	registerParameter(state, "n", (voidP) new uint(100), ECF::INT,
		"number of antibodies cloned every generation (default: 100)");
	registerParameter(state, "beta", (voidP) new double(1), ECF::DOUBLE,
		"number of clones (percentage) for every antibody (default: 1.0)");
	registerParameter(state, "c", (voidP) new double(0.2), ECF::DOUBLE,
		"mutation rate (default: 0.2)");
	registerParameter(state, "d", (voidP) new double(0.0), ECF::DOUBLE,
		"fraction of population regenerated every generation (default: 0)");
	registerParameter(state, "cloningVersion", (voidP) new std::string("proportional"), ECF::STRING,
		"cloning version, static or proportional (default: proportional)");
	registerParameter(state, "selectionScheme", (voidP) new std::string("CLONALG1"), ECF::STRING,
		"which selection scheme to use, CLONALG1 or CLONALG2 (default: CLONALG1)");
}


bool Clonalg::initialize(StateP state)
{		
	voidP lBound = state->getGenotypes()[0]->getParameterValue(state, "lbound");
	lbound = *((double*) lBound.get());

	voidP uBound = state->getGenotypes()[0]->getParameterValue(state, "ubound");
	ubound = *((double*) uBound.get());

	voidP dimension_ = state->getGenotypes()[0]->getParameterValue(state, "dimension");
	dimension = *((uint*) dimension_.get());
	
	
	voidP populationSize_ = state->getRegistry()->getEntry("population.size");
	uint populationSize = *((uint*) populationSize_.get());

	voidP n_ = getParameterValue(state, "n");
	n = *((uint*) n_.get());
	if( n<1 || n>populationSize) {
		ECF_LOG(state, 1, "Error: CLONALG requires parameter 'n' to be an integer in range <0, population.size] ");
		throw "";}


	voidP beta_ = getParameterValue(state, "beta");
	beta = *((double*) beta_.get());
	if( beta <= 0 ) {
		ECF_LOG(state, 1, "Error: CLONALG requires parameter 'beta' to be a double greater than 0");
		throw "";}


	voidP c_ = getParameterValue(state, "c");
	c = *((double*) c_.get());
	if( c <= 0 ) {
		ECF_LOG(state, 1, "Error: CLONALG requires parameter 'c' to be a double greater than 0");
		throw "";}


	voidP d_ = getParameterValue(state, "d");
	d = *((double*) d_.get());
	if( d<0 || d>1 ) {
		ECF_LOG(state, 1, "Error: CLONALG requires parameter 'd' to be a double in range [0, 1] ");
		throw "";}

	voidP cloning_ = getParameterValue(state, "cloningVersion");
	cloningVersion = *((std::string*) cloning_.get());
	if( cloningVersion != "static" && cloningVersion != "proportional"  ) {
		ECF_LOG(state, 1, "Error: CLONALG requires parameter 'cloningVersion' to be either 'static' or a 'proportional'");
		throw "";}

	voidP selection_ = getParameterValue(state, "selectionScheme");
	selectionScheme = *((std::string*) selection_.get());
	if( selectionScheme != "CLONALG1" && selectionScheme != "CLONALG2"  ) {
		ECF_LOG(state, 1, "Error: CLONALG requires parameter 'selectionScheme' to be either 'CLONALG1' or 'CLONALG2'");
		throw "";}
				

	// algorithm accepts a single FloatingPoint or Binary genotype 
	// or a genotype derived from the abstract RealValueGenotype class
	GenotypeP activeGenotype = state->getGenotypes()[0];
	RealValueGenotypeP rv = std::dynamic_pointer_cast<RealValueGenotype> (activeGenotype);
	if(!rv) {
		ECF_LOG_ERROR(state, "Error: CLONALG algorithm accepts only a RealValueGenotype derived genotype! (FloatingPoint or Binary)");
		throw ("");
	}

	// batch run check
	if(areGenotypesAdded_)
		return true;

	//add parentAntibody genotype
	if( selectionScheme == "CLONALG1"){
		FloatingPointP flpoint[2];
		for(uint iGen = 1; iGen < 2; iGen++) {
			flpoint[iGen] = (FloatingPointP) new FloatingPoint::FloatingPoint;
			state->setGenotype(flpoint[iGen]);

			flpoint[iGen]->setParameterValue(state, "dimension", (voidP) new uint(1));					

			// initial value of age parameter should be (or as close as possible to) 0				
			flpoint[iGen]->setParameterValue(state, "lbound", (voidP) new double(0));
			flpoint[iGen]->setParameterValue(state, "ubound", (voidP) new double(0.01));
			
		}
		ECF_LOG(state, 1, "CLONALG algorithm: added 1 FloatingPoint genotype (parentAntibody)");
	}

	// mark adding of genotypes
	areGenotypesAdded_ = true;

    return true;
}


bool Clonalg::advanceGeneration(StateP state, DemeP deme)
{	
	  std::vector<IndividualP> clones;
	  if (selectionScheme == "CLONALG1")
		 markAntibodies(deme);
	  cloningPhase(state, deme, clones);
	  hypermutationPhase(state, deme, clones);
	  selectionPhase(state, deme, clones);
      birthPhase(state, deme, clones);
	  replacePopulation(state, deme, clones);
	 
      return true;
}


//! mark antibodies so the alg can know which clone belongs to which parent Antibody
bool Clonalg::markAntibodies(DemeP deme)
{
	for( uint i = 0; i < deme->getSize(); i++ ) { // for each antibody
		RealValueGenotypeP flp = std::static_pointer_cast<RealValueGenotype> (deme->at(i)->getGenotype(1));
		double &parentAb = flp->realValue[0];
		parentAb = (double)i;				
	}
	return true;
}


bool Clonalg::cloningPhase(StateP state, DemeP deme, std::vector<IndividualP> &clones)
{	
	// calculate number of clones per antibody
	uint clonesPerAntibody = (uint) (beta * deme->getSize());

	// storing all antibodies in a vector
	for( uint i = 0; i < deme->getSize(); i++ )  // for each antibody	
		clones.push_back(deme->at(i));

	// sorting all antibodies
	std::sort (clones.begin(), clones.end(), sortPopulationByFitness);
	
	// leaving n best antibodies for cloning
	clones.erase (clones.begin()+n,clones.end());
	
	for( uint i = 0; i < n; i++ ) { // for each antibody in clones vector
		IndividualP antibody = clones.at(i);
	
		//static cloning : cloning each antibody beta*populationSize times
		if (cloningVersion == "static") {
			for (uint j = 0; j < clonesPerAntibody; j++) 
				clones.push_back(copy(antibody));
		}

		//proportional cloning 
		else { 
			uint proportionalCloneNo = clonesPerAntibody/(i+1);
			for (uint j = 0; j < proportionalCloneNo ; j++) 
				clones.push_back(copy(antibody));
		}
    }
	
	return true;
}


bool Clonalg::hypermutationPhase(StateP state, DemeP deme, std::vector<IndividualP> &clones)
{			
	// sorting all antibodies
	std::sort (clones.begin(), clones.end(), sortPopulationByFitness);

	uint M;	// M - number of mutations of a single antibody 
	uint k;

	// calculate number of clones per antibody
	uint clonesPerAntibody = (uint) (beta * deme->getSize());

	// these get used in case of proportional cloning
	uint counter = 0;		
	uint parentIndex = 0;
	
	for( uint i = 0; i < clones.size(); i++ ) { // for each antibody in vector clones
		IndividualP antibody = clones.at(i);
		
		RealValueGenotypeP flp = std::static_pointer_cast<RealValueGenotype> (antibody->getGenotype(0));
	    std::vector< double > &antibodyVars = flp->realValue;
		
		// inversely proportional hypermutation (better antibodies are mutated less)
		if (cloningVersion == "static")
			k = 1+ i/(clonesPerAntibody +1);
		else{
			if (counter == i) {
				parentIndex++;
				counter += 1 + clonesPerAntibody/parentIndex;
			}					
			k = parentIndex;
		}

		M = (int) ((1- 1/(double)(k)) * (c*dimension) + (c*dimension));
						
		// mutate M times
		for (uint j = 0; j < M; j++) {
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
		evaluate(antibody);
	}		
	return true;
}


bool Clonalg::selectionPhase(StateP state, DemeP deme, std::vector<IndividualP> &clones)
{	
	if( selectionScheme == "CLONALG1") {
		
		std::sort (clones.begin(), clones.end(), sortPopulationByParentAndFitness);
	
		int formerParent = -1;
		std::vector<IndividualP> temp_clones;

		for (uint i = 0; i < clones.size(); i++) {
			RealValueGenotypeP flp = std::static_pointer_cast<RealValueGenotype> (clones.at(i)->getGenotype(1));
			double &parentAb = flp->realValue[0];
			if (formerParent != parentAb) {
				temp_clones.push_back(clones.at(i));
				formerParent = (int) parentAb;
			}
		}
		clones = temp_clones;				
	}

	std::sort (clones.begin(), clones.end(), sortPopulationByFitness);
	uint selNumber = (uint)((1-d)*deme->getSize());

	//keep best (1-d)*populationSize antibodies ( or all if the number of clones is less than that )
	if(selNumber < clones.size())
		clones.erase (clones.begin()+ selNumber, clones.end());

	return true;
}


bool Clonalg::birthPhase(StateP state, DemeP deme, std::vector<IndividualP> &clones)
{	
	//  birthNumber - number of new antibodies randomly created and added 
	uint birthNumber = (uint) (deme->getSize() - clones.size());
	
	IndividualP newAntibody = copy(deme->at(0));
	RealValueGenotypeP flp = std::static_pointer_cast<RealValueGenotype> (newAntibody->getGenotype(0));

	for (uint i = 0; i<birthNumber; i++) {
		//create a random antibody
		flp->initialize(state);
		evaluate(newAntibody);

		//add it to the clones vector
		clones.push_back(copy(newAntibody));
	}			
	return true;
}


//! replace population with the contents of clones vector
bool Clonalg::replacePopulation(StateP state, DemeP deme, std::vector<IndividualP> &clones)
{
	for( uint i = 0; i < clones.size(); i++ ) // for each antibody
		deme->replace(i, clones.at(i));

	clones.clear();

	return true;
}
