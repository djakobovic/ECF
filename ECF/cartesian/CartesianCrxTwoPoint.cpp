#include "CartesianCrxTwoPoint.h"
#include "Cartesian_c.h"
#include "Function.h"
#include "FunctionSet.h"

namespace CGP {

    void CartesianCrxTwoPoint::registerParameters(StateP state)
    {
        myGenotype_->registerParameter(state, "crx.twopoint", (voidP) new double(0), ECF::DOUBLE);
    }


    bool CartesianCrxTwoPoint::initialize(StateP state)
    {
        voidP sptr = myGenotype_->getParameterValue(state, "crx.twopoint");
        probability_ = *((double*)sptr.get());
        return true;
    }


    bool CartesianCrxTwoPoint::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
    {
        //std::cout << "Cartesian crossover two-points " << std::endl;
        Cartesian* p1 = (Cartesian*) (gen1.get());
        Cartesian* p2 = (Cartesian*) (gen2.get());
        Cartesian* ch = (Cartesian*) (child.get());
        ch->clear();
        int outputs1 = p1->nOutputs;
        int outputs2 = p2->nOutputs;
        vector<int> blocks1;
        vector<int> blocks2;
        vector<FunctionP> v1 = ((FunctionSet*)p1->functionSet.get())->vFunctions;
        vector<FunctionP> v2 = ((FunctionSet*)p2->functionSet.get())->vFunctions;
        for(int i = 0; i < p1->size() - outputs1; i++) {
            blocks1.push_back(i);
            i += v1[p1->operator[](i)]->getNumberOfArguments();
        }
        for(int i = 0; i < p2->size() - outputs2; i++) {
            blocks2.push_back(i);
            i += v2[p2->operator[](i)]->getNumberOfArguments();
        }
        int cutoff = state_->getRandomizer()->getRandomInteger(blocks1.size());
        int secondcutbegin = 0;
        for(int i = 0; i < blocks2.size(); i++) {
            if(blocks2[i] >= cutoff) {
                secondcutbegin = blocks2[i];

                break;
            }
        }
        int secondcutend = 0;
        int firstcutcontinuation = 0;
        while(blocks2[secondcutend] <= blocks2[secondcutbegin]) {
            secondcutend = state_->getRandomizer()->getRandomInteger(blocks2.size());
        }
        for(int i = 0; i < blocks1.size(); i++) {
            if(blocks1[i] >= secondcutend) {
                firstcutcontinuation = blocks1[i];
                break;
            }
        }
        int counter1 = 0;
        int counter2 = 0;

        for(int i = 0; i < cutoff; i++) {
            ch->push_back(p1->at(i));
        }
        for(int i = secondcutbegin; i < secondcutend; i++) {

        }

        return true;
    }

}

