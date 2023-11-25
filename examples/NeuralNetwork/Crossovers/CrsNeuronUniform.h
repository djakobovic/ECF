#ifndef CRSNEURONUNIFORM_H_
#define CRSNEURONUNIFORM_H_
#include <ecf/ECF.h>
// #include "NeuralNetwork.h"

// {

/**
 * \ingroup genotypes flpoint
 * \brief NeuralNetwork genotype: crossover operator that crosses neurons between parents with approximately equal number of neurons from each parent
 */
class CrsNeuronUniform: public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef std::shared_ptr<CrsNeuronUniform> CrsNeuronUniformP;
// }
#endif /* CRSNEURONUNIFORM_H_ */
