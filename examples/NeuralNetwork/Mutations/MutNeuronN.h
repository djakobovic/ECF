#ifndef MUTNEURONN_H_
#define MUTNEURONN_H_
#include <ecf/ECF.h>
// #include "NeuralNetwork.h"

// {

/**
 * \ingroup genotypes flpoint
 * \brief NeuralNetwork genotype: Mutation where all weights of n randomly chosen neurons are changed by adding a random number
 */
class MutNeuronN: public MutationOp
{
protected:
public:
	bool mutate(GenotypeP gene);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef std::shared_ptr<MutNeuronN> MutNeuronNP;
// }
#endif /* MUTNEURONN_H_ */
