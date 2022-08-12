#ifndef CRSNEURON_H_
#define CRSNEURON_H_
#include <ecf/ECF.h>
// #include "NeuralNetwork.h"

// {

/**
 * \ingroup genotypes flpoint
 * \brief NeuralNetwork genotype: crossover that takes all weights from a neuron chosen from one of the two parents (for each neuron)
 */
class CrsNeuron: public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<CrsNeuron> CrsNeuronP;
// }
#endif /* CRSNEURON_H_ */
