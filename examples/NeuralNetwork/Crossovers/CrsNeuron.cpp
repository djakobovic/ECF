#include <ecf/ECF.h>
#include <cmath>
#include "CrsNeuron.h"
#include "../NeuralNetwork.h"

// {

void CrsNeuron::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.neuron", (voidP) new double(0), ECF::DOUBLE);
}


bool CrsNeuron::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.neuron");
	probability_ = *((double*)sptr.get());
	return true;
}

bool CrsNeuron::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	NeuralNetwork* p1 = (NeuralNetwork*) (gen1.get());
	NeuralNetwork* p2 = (NeuralNetwork*) (gen2.get());
	NeuralNetwork* ch = (NeuralNetwork*) (child.get());

	vector <uint> structure = p1->getLayerStructure();
	// uint noLayers = p1->getNoLayers();

	for (uint i = 1; i<structure.size(); i++) {
        for (uint j = 0; j < structure[i]; j++) {
            vector <double> weights (structure[i-1] + 1);
            switch (state_->getRandomizer()->getRandomInteger(0, 1)) {
            case 0:
                p1->getWeights(i+1, j+1, weights);
                ch->setWeights(i+1, j+1, weights);
								break;
            case 1:
                p2->getWeights(i+1, j+1, weights);
                ch->setWeights(i+1, j+1, weights);
                break;
            }
        }
	}
	return true;
}
// }
