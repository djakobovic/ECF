#include "AlgParticleSwarmOptimization.h"


ParticleSwarmOptimization::ParticleSwarmOptimization()
{
	// define algorithm name
	name_ = "ParticleSwarmOptimization";

	areGenotypesAdded_ = false;

	// create selection operators needed
	// in this case, SelBestOp
	selBestOp = static_cast<SelectionOperatorP> (new SelBestOp);
}


// register algorithm parameters
void ParticleSwarmOptimization::registerParameters(StateP state)
{
	registerParameter(state, "weightType", (voidP) new InertiaWeightType(CONSTANT), ECF::INT,
		"weight type update: 0 - constant, 1 - time dependant (based on max generations)");
	registerParameter(state, "weight", (voidP) new double(0.8), ECF::DOUBLE,
		"initial inertia weight (either constant or time dependant)");
	registerParameter(state, "maxVelocity", (voidP) new double(10.0), ECF::DOUBLE,
		"max particle velocity");
	registerParameter(state, "bounded", (voidP) new uint(0), ECF::UINT, 
		"should the algorithm respect the bounds defined in the genotype or not (default: 0)");
}


bool ParticleSwarmOptimization::initialize(StateP state)
{
	// initialize all operators
	selBestOp->initialize(state);

	voidP weightType = getParameterValue(state, "weightType");
	m_weightType = *((InertiaWeightType*) weightType.get());

	voidP weight = getParameterValue(state, "weight");
	m_weight = *((double*) weight.get());

	voidP maxV = getParameterValue(state, "maxVelocity");
	m_maxV = *((double*) maxV.get());

	// test if inertia weight type is time variant and if so, check if max iterations specified
	if(m_weightType == TIME_VARIANT) {
		if(state->getRegistry()->isModified("term.maxgen")) {
			// read maxgen parameter
			m_maxIter = *(boost::static_pointer_cast<int>( state->getRegistry()->getEntry("term.maxgen") ));
		}
		else {
			ECF_LOG_ERROR(state, "Error: term.maxgen has to be specified in order to use time variant inertia eight in PSO algorithm");
			throw("");
		}
	}

	// algorithm accepts a single FloatingPoint or Binary genotype 
	// or a genotype derived from the abstract RealValueGenotype class
	GenotypeP activeGenotype = state->getGenotypes()[0];
	RealValueGenotypeP rv = boost::dynamic_pointer_cast<RealValueGenotype> (activeGenotype);
	if(!rv || state->getGenotypes().size() > 1) {
		ECF_LOG_ERROR(state, "Error: PSO algorithm accepts only a RealValueGenotype derived genotype! (FloatingPoint or Binary)");
		throw ("");
	}

	voidP sptr = state->getGenotypes()[0]->getParameterValue(state, "dimension");
	uint numDimension = *((uint*) sptr.get());

	voidP bounded = getParameterValue(state, "bounded");
	bounded_ = *((bool*) bounded.get());

	sptr = state->getGenotypes()[0]->getParameterValue(state, "lbound");
	lbound_ = *((double*) sptr.get());

	sptr = state->getGenotypes()[0]->getParameterValue(state, "ubound");
	ubound_ = *((double*) sptr.get());

	// batch run check
	if(areGenotypesAdded_)
		return true;

	FloatingPointP flpoint[4];
	for(uint iGen = 1; iGen < 4; iGen++) {

		flpoint[iGen] = (FloatingPointP) new FloatingPoint::FloatingPoint;
		state->setGenotype(flpoint[iGen]);

		if(iGen == 3)
			flpoint[iGen]->setParameterValue(state, "dimension", (voidP) new uint(1));
		else
			flpoint[iGen]->setParameterValue(state, "dimension", (voidP) new uint(numDimension));

		// other parameters are proprietary (ignored by the algorithm)
		flpoint[iGen]->setParameterValue(state, "lbound", (voidP) new double(0));
		flpoint[iGen]->setParameterValue(state, "ubound", (voidP) new double(1));
	}
	ECF_LOG(state, 1, "PSO algorithm: added 3 FloatingPoint genotypes (particle velocity, best-so-far postition, best-so-far fitness value)");

	// mark adding of genotypes
	areGenotypesAdded_ = true;

	return true;
}


bool ParticleSwarmOptimization::advanceGeneration(StateP state, DemeP deme)
{
//       a) For each particle:
//          1) If the fitness value is better than the best fitness value (pBest) in  history
//          2) Set current value as the new pBest
//          End
//       b) For each particle:
//          1) Find, in the particle neighborhood, the particle with the best fitness
//          2) Calculate particle velocity according to the velocity equation (1)
//          3) Apply the velocity constriction
//          4) Update particle position according to the position equation (2)
//          5) Apply the position constriction
//          6) Calculate fitness value
//          End

	// a)
	for( uint i = 0; i < deme->getSize(); i++ ) { // for each particle 
		IndividualP particle = deme->at(i);

		// the whole point of this section is to compare fitness and pbest
		FloatingPointP flp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (particle->getGenotype(3));
		double &particlePbestFitness = flp->realValue[0];
		double fitness = particle->fitness->getValue();

		flp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (particle->getGenotype(0));
		std::vector< double > &positions = flp->realValue;

		flp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (particle->getGenotype(2));
		std::vector< double > &pbestx = flp->realValue;

		// set particle pbestx-es
		if( /*iter == 0 ||*/ fitness < particlePbestFitness ) { // minimize error
			particlePbestFitness = fitness;

			// set pbestx-es
			for( uint j = 0;j<pbestx.size();j++ ) {
				pbestx[j] = positions[j];
			}
		}
		// NOTE store best particle index?
	}

	// b)
	for( uint i = 0; i < deme->getSize(); i++ ) { // for each particle
		IndividualP particle = deme->at(i);

		IndividualP bestParticle = selBestOp->select( *deme );

		FloatingPointP flp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (particle->getGenotype(0));
		std::vector< double > &positions = flp->realValue;

		flp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (particle->getGenotype(1));
		std::vector< double > &velocities = flp->realValue;

		flp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (particle->getGenotype(2));
		std::vector< double > &pbestx = flp->realValue;


		double weight_up;

		switch( m_weightType )
		{
			//time variant weight, linear from weight to 0.4
			case TIME_VARIANT:
			weight_up = ( m_weight - 0.4 ) * ( m_maxIter - state->getGenerationNo() ) / m_maxIter + 0.4;
			break;

			// constant inertia weight
			case CONSTANT:
			default:
			weight_up = m_weight;
			break;
		}
		// calculate particle velocity according to the velocity equation (1)
		flp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (bestParticle->getGenotype(2));
		std::vector< double > &bestParticlesPbestx = flp->realValue;
		for( uint j = 0; j < velocities.size(); j++ ) {
			double velocity;

			velocity = weight_up * velocities[j] +
			   2 * (rand()/(float)RAND_MAX) * (pbestx[j] - positions[j]) +
			   2 * (rand()/(float)RAND_MAX) * (bestParticlesPbestx[j] - positions[j]);
			if( velocity > m_maxV ) velocity = m_maxV;
			velocities[j] = velocity;

			positions[j] += velocities[j];
			// TODO apply position constriction

			// check for bounds
			if(bounded_) {
				if(positions[j] < lbound_)
					positions[j] = lbound_;
				if(positions[j] > ubound_)
					positions[j] = ubound_;
			}
		}

		// determine new particle fitness
		evaluate( particle );
	}

	return true;
}
