#ifndef CRSLAYER_H_
#define CRSLAYER_H_
#include <ecf/ECF.h>
// #include "NeuralNetwork.h"

// {

/**
 * \ingroup genotypes flpoint
 * \brief NeuralNetwork genotype: crossover operator that takes weights from whole layers between two parents
 */
class CrsLayer: public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<CrsLayer> CrsLayerP;
// }
#endif /* CRSLAYER_H_ */
