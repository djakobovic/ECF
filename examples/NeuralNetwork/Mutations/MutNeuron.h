#ifndef MUTNEURON_H_
#define MUTNEURONH_
#include <ecf/ECF.h>
// #include "NeuralNetwork.h"

// {

/**
 * \ingroup genotypes flpoint
 * \brief NeuralNetwork genotype: Mutation where all weights of a randomly chosen neuron are changed by adding a random number
 */
class MutNeuron: public MutationOp
{
protected:
public:
	bool mutate(GenotypeP gene);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<MutNeuron> MutNeuronP;
// }
#endif /* MUTNEURON_H_ */
