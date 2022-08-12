#include <ecf/ECF.h>
#include <cmath>
#include "CrsNeuronUniform.h"
#include "../NeuralNetwork.h"

// {

void CrsNeuronUniform::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.neuronUniform", (voidP) new double(0), ECF::DOUBLE);
}


bool CrsNeuronUniform::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.neuronUniform");
	probability_ = *((double*)sptr.get());
	return true;
}

bool CrsNeuronUniform::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
{
    NeuralNetwork* p1 = (NeuralNetwork*) (gen1.get());
	NeuralNetwork* p2 = (NeuralNetwork*) (gen2.get());
	NeuralNetwork* ch = (NeuralNetwork*) (child.get());

	vector <uint> structure = p1->getLayerStructure();
	uint noNeuron = 0;
	for (uint i=1; i<structure.size(); i++) {
        noNeuron += structure[i];
	}
	bool odd = true;
	if (noNeuron % 2 == 0){
        odd = false;
	}
	uint noP1 , noP2;
	noP1 = noP2 = noNeuron/2;
	if (odd) {
        switch (state_->getRandomizer()->getRandomInteger(0, 1)) {
            case 0:
                noP1 += 1;
                break;
            case 1:
                noP2 += 1;
                break;
        }
	}

    uint j = 0;
    vector <double> weights;
	for (uint i = 1; i<structure.size(); i++) {
        while(j < structure[i]) {
            switch (state_->getRandomizer()->getRandomInteger(0, 1)) {
            case 0:
                {
                if (noP1 == 0) {
                    break;
                }
                p1->getWeights(i+1, j+1, weights);
                ch->setWeights(i+1, j+1, weights);
                noP1--;
                j++;
				break;
                }
            case 1:
                {
                if (noP2 == 0) {
                    break;
                }
                p2->getWeights(i+1, j+1, weights);
                ch->setWeights(i+1, j+1, weights);
                noP2--;
                j++;
                break;
                }
            }
        }
	}
	return true;
}
// }
