#include "ECF_base.h"
#include "ECF_derived.h"
#include "ECF_macro.h"
#include "AlgArtificialBeeColony.h"


ArtificialBeeColony::ArtificialBeeColony()
{
	// define algorithm name
	name_ = "ArtificialBeeColony";

	// create selection operators needed
	// in this case, selRandomOp and selFitOp
	selRandomOp = (SelRandomOpP) (new SelRandomOp);
	selBestOp = (SelBestOpP) (new SelBestOp);
	selWorstOp = (SelWorstOpP) (new SelWorstOp);
	selFitOp = (SelFitnessProportionalOpP) (new SelFitnessProportionalOp);

	isTrialAdded_ = false;
	elitism_ = true;
}


//register any parameters
void ArtificialBeeColony::registerParameters(StateP state)
{
	// limit is a maximum number of cycles for each individual	
	registerParameter(state, "limit", (voidP) new uint(300), ECF::INT, 
		"Maximum number of cycles for each individual (default: 300)");
	// elitism: should the current best individual be preserved
	registerParameter(state, "elitism", (voidP) new uint(1), ECF::INT, 
		"Elitism: the current best food source is preserved (default: 1)");
}


bool ArtificialBeeColony::initialize(StateP state)
{
	// initialize all operators
	selFitOp->initialize(state);
	selFitOp->setSelPressure(2);
	selBestOp->initialize(state);
	selWorstOp->initialize(state);
	selRandomOp->initialize(state);

	voidP sptr = state->getRegistry()->getEntry("population.size");
	uint size = *((uint*) sptr.get());
	probability_.resize(size);

	// algorithm accepts a single FloatingPoint or Binary genotype 
	// or a genotype derived from the abstract RealValueGenotype class
	GenotypeP activeGenotype = state->getGenotypes()[0];
	RealValueGenotypeP rv = std::dynamic_pointer_cast<RealValueGenotype> (activeGenotype);
	if(!rv) {
		ECF_LOG_ERROR(state, "Error: ABC algorithm accepts only a RealValueGenotype derived genotype! (FloatingPoint or Binary)");
		throw ("");
	}

	voidP limitp = getParameterValue(state, "limit");
	limit_ = *((uint*) limitp.get());

	if( (int)limit_ < 0 ) {
		ECF_LOG(state, 1, "Error: ABC algorithm requires parameter 'limit' to be a nonnegative value!");
		throw "";
	}

	voidP elitismp = getParameterValue(state, "elitism");
	elitism_ = *((bool*) elitismp.get());

	voidP lBound = state->getGenotypes()[0]->getParameterValue(state, "lbound");
	lbound_ = *((double*) lBound.get());
	voidP uBound = state->getGenotypes()[0]->getParameterValue(state, "ubound");
	ubound_ = *((double*) uBound.get());

	// batch run check
	if(isTrialAdded_)
		return true;

	FloatingPointP flpoint[2];
	for(uint iGen = 1; iGen < 2; iGen++) {

		flpoint[iGen] = (FloatingPointP) new FloatingPoint::FloatingPoint;
		state->setGenotype(flpoint[iGen]);

		flpoint[iGen]->setParameterValue(state, "dimension", (voidP) new uint(1));

		// initial value of trial parameter should be (as close as possible to) 0				
		flpoint[iGen]->setParameterValue(state, "lbound", (voidP) new double(0));
		flpoint[iGen]->setParameterValue(state, "ubound", (voidP) new double(0.01));
	}
	ECF_LOG(state, 1, "ABC algorithm: added 1 FloatingPoint genotype (trial)");

	// mark adding of trial genotype
	isTrialAdded_ = true;

    return true;
}


bool ArtificialBeeColony::advanceGeneration(StateP state, DemeP deme)
{
//			In ABC, a colony of artificial bees search for artificial food sources (good solutions for a given problem):
//          The colony of artificial bees contains three groups of bees: employed bees, onlooker bees, and scout bees 
//			- employed bees are associated with specific food sources
//			- onlooker bees watch the dance of employed bees within the hive to choose a food source
//			- scout bees search for food sources randomly
//
//			Initially, a randomly distributed initial population (food sources) is generated
//			REPEAT 
//			1)	Employed Bees Phase
//				1a)	for each food source createNewFoodSource() is called			
//
//			2)	Onlooker Bees Phase
//				2a)	each onlooker bee chooses a food source depending on their fitness values (better individuals are more likely to be chosen)
//				2b) for each chosen food source createNewFoodSource() is called	
//	
//			3)	Scout Bees Phase
//				** as a rule, none or one food source gets abandoned per generation
//				3a) for each food source get trial variable
//				3b) remember the source if its trial exceeded limit 
//				3c) if there is a source that exceeded limit, replace it with a random food source
//
//			UNTIL(requirements are met)
//
//			*createNewFoodSource()
//				a)	for each food source find a neighbour (a random food source in the population) 
//				b)	produce a modification on the food source (discover a new food source)
//				c)	evaluate new food source 
//				d)	if the fitness value of the new one is better than that of the original source,
//					memorize the new source, forget the old one and set trial to 0
//					otherwise keep the old one and increment trial


	employedBeesPhase(state, deme);
	onlookerBeesPhase(state, deme);	
	scoutBeesPhase(state, deme);

	return true;
}


bool ArtificialBeeColony::employedBeesPhase(StateP state, DemeP deme)
{
	for( uint i = 0; i < deme->getSize(); i++ ) { // for each food source
		IndividualP food = deme->at(i);
		createNewFoodSource(food, state, deme);
	}
	return true;
}


bool ArtificialBeeColony::onlookerBeesPhase(StateP state, DemeP deme)
{
	// choose a food source depending on its fitness value (better individuals are more likely to be chosen)
	// calculate selection probabilities, rotate individuals
	calculateProbabilities(state, deme);
	int demeSize = (int) deme->getSize();
	int i = state->getRandomizer()->getRandomInteger(demeSize);
	int n = 0;
	while( n < demeSize) {
		int fact = i++ % demeSize;
		IndividualP food = deme->at(fact);
		
		if (state->getRandomizer()->getRandomDouble() < probability_[fact]){
			n++;
			createNewFoodSource(food, state, deme);
		}			
	}

	return true;
}


bool ArtificialBeeColony::calculateProbabilities(StateP state, DemeP deme)
{
	IndividualP bestFood = selBestOp->select(*deme);
	double bestFitness = bestFood->fitness->getValue();
	double worstFitness = selWorstOp->select(*deme)->fitness->getValue();
	double offset = 0;

	// scale fitness values
	if(bestFitness < worstFitness && bestFitness < 0)
		offset = 0.1 - bestFitness;	// minimization
	else if(worstFitness < 0)
		offset = 0.1 - worstFitness;	// maximization

	// for each food source
	for( uint i = 0; i < deme->getSize(); i++ ) {
		IndividualP food = deme->at(i);
		double thisFitness = food->fitness->getValue();

		if (bestFitness == thisFitness)
			probability_[i] = 1.0;
		else if (thisFitness < bestFitness) // maximization problems
			probability_[i] = 0.1 + 0.9 * (thisFitness + offset) / (bestFitness + offset);
		else								// minimization problems
			probability_[i] = 0.1 + 0.9 * (bestFitness + offset) / (thisFitness + offset);

		// using selFitPropOp method
		//probability_[i] = 0.1 + 0.9 * (thisFitness - worstFitness)/(bestFitness - worstFitness);
	}
	return true;
}



bool ArtificialBeeColony::scoutBeesPhase(StateP state, DemeP deme)
{
	IndividualP unimproved;
	IndividualP bestFood = selBestOp->select(*deme);

	double maxTrial = 0;
	for( uint i = 0; i < deme->getSize(); i++ ) { // for each food source
		IndividualP food = deme->at(i);

		if(food == bestFood)
			continue;

		// get food source's trial variable
		FloatingPointP flp = std::static_pointer_cast<FloatingPoint::FloatingPoint> (food->getGenotype(1));
		double &trial = flp->realValue[0];

		// remember the source if its trial exceeded limit 
		if (trial > limit_ && trial > maxTrial){
			unimproved = food;
			maxTrial = trial;
		}					
	}

	// if there is a  food source that exceeded the limit, replace it with a random one
	if (unimproved != NULL){
		FloatingPointP flp = std::static_pointer_cast<FloatingPoint::FloatingPoint> (unimproved->getGenotype(1));
		double &trial = flp->realValue[0];
		trial = 0;
		flp = std::static_pointer_cast<FloatingPoint::FloatingPoint> (unimproved->getGenotype(0));
		flp->initialize(state);
		evaluate(unimproved);
	}

	return true;
}


bool ArtificialBeeColony::createNewFoodSource(IndividualP food, StateP state, DemeP deme)
{
	// for each food source find a neighbour 
	IndividualP neighbour;
	do{
		neighbour = selRandomOp->select(*deme);
	}while(food->index == neighbour->index);

	// potential new food source
	IndividualP newFood = copy(food);

	FloatingPointP flp = std::static_pointer_cast<FloatingPoint::FloatingPoint> (food->getGenotype(0));
	std::vector< double > &foodVars = flp->realValue;
	flp = std::static_pointer_cast<FloatingPoint::FloatingPoint> (neighbour->getGenotype(0));
	std::vector< double > &neighbourVars = flp->realValue;
	flp = std::static_pointer_cast<FloatingPoint::FloatingPoint> (newFood->getGenotype(0));
	std::vector< double > &newFoodVars = flp->realValue;


	uint param = state->getRandomizer()->getRandomInteger((int)foodVars.size());
	double factor = state->getRandomizer()->getRandomDouble();
	double value = foodVars[param] * (1 - 2 * factor) * (foodVars[param] - neighbourVars[param]);
	if (value > ubound_)
		value = ubound_;
	else if (value < lbound_)
		value = lbound_;

	// produce a modification on the food source (discover a new food source)
	newFoodVars[param] = value;
	evaluate(newFood);

	flp = std::static_pointer_cast<FloatingPoint::FloatingPoint> (food->getGenotype(1));
	double &foodTrial = flp->realValue[0];

	//	d)	if the fitness value of the new food source is better than that of the original source,
	//		memorize the new source, forget the old one and set trial to 0
	//		otherwise keep the old one and increment trial
	if(newFood->fitness->isBetterThan(food->fitness) )
	{
		foodVars[param] = value;
		evaluate(food);
		foodTrial = 0;
	}
	else {
		foodTrial += 1;
	}
	return true;
}