#include <ecf/ECF.h>
#include <cmath>
#include "CrsLayer.h"
#include "../NeuralNetwork.h"

// {

void CrsLayer::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.layer", (voidP) new double(0), ECF::DOUBLE);
}


bool CrsLayer::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.layer");
	probability_ = *((double*)sptr.get());
	return true;
}

bool CrsLayer::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
	NeuralNetwork* p1 = (NeuralNetwork*) (gen1.get());
	NeuralNetwork* p2 = (NeuralNetwork*) (gen2.get());
	NeuralNetwork* ch = (NeuralNetwork*) (child.get());

	vector <uint> structure = p1->getLayerStructure();
	uint noLayers = p1->getNoLayers();

	for (uint i = 1; i<noLayers; i++) {
        switch (state_->getRandomizer()->getRandomInteger(0, 1)) {
        case 0:
            for (uint j = 0; j < structure[i]; j++) {
                vector <double> weights (structure[i-1] + 1);
                p1->getWeights(i+1, j+1, weights);
                ch->setWeights(i+1, j+1, weights);
            }
            break;
        case 1:
            for (uint j = 0; j < structure[i]; j++) {
                vector <double> weights (structure[i-1] + 1);
                p2->getWeights(i+1, j+1, weights);
                ch->setWeights(i+1, j+1, weights);
            }
            break;
        }
    }
	return true;
}
// }
