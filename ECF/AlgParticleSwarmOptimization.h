#ifndef Particle_Swarm_Optimization_h
#define Particle_Swarm_Optimization_h

#include "ECF_base.h"
#include "ECF_macro.h"

#include "Algorithm.h"
#include "floatingpoint/FloatingPoint.h"


/**
 * \brief Particle swarm optimization algorithm (see e.g. http://en.wikipedia.org/wiki/Particle_swarm_optimization)
 * \ingroup algorithms serial
 * 
 * PSO algorithm accepts only a single FloatingPoint genotype (vector of real values).
 * Additionally, it adds the following genotypes for algorithm implementation:
 * - FloatingPoint genotype as particle velocity
 * - FloatingPoint genotype as best-so-far position
 * - FloatingPoint genotype as best-so-far fitness value
 */
class ParticleSwarmOptimization : public Algorithm
{
public:
	ParticleSwarmOptimization();

	enum InertiaWeightType
	{
		CONSTANT,
		TIME_VARIANT
	};

	bool advanceGeneration(StateP state, DemeP deme);
	bool initialize(StateP state);
	void registerParameters(StateP state);

protected:
	SelectionOperatorP selBestOp;

	int m_maxIter;
	InertiaWeightType m_weightType;
	double m_weight;
	double m_maxV;
	bool areGenotypesAdded_;
	bool bounded_;	//!< constrained or not
	double lbound_, ubound_;
};
typedef boost::shared_ptr<ParticleSwarmOptimization> ParticleSwarmOptimizationP;

#endif // Particle_Swarm_Optimization_h

