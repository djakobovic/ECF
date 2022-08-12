#include <cmath>
#include "../../ECF.h"
#include "ApproxEvalOp.h"

void AproxEvalOp::registerParameters(StateP state)
{
        state->getRegistry()->registerEntry("iksovi", (voidP) (new std::string), ECF::STRING);
        state->getRegistry()->registerEntry("ipsiloni", (voidP) (new std::string), ECF::STRING);
}


bool AproxEvalOp::initialize(StateP state)
{
        if(!state->getRegistry()->isModified("iksovi") || !state->getRegistry()->isModified("ipsiloni")) return false;
        double pom;
        std::stringstream sx, sy;

        voidP sptr = state->getRegistry()->getEntry("iksovi"); // uzmi ikseve
        sx << *((std::string*) sptr.get()); 
        while(sx >> pom) x.push_back(pom);
        sptr = state->getRegistry()->getEntry("ipsiloni");     // uzmi ipsilone
        sy << *((std::string*) sptr.get());
        while(sy >> pom) y.push_back(pom);

        if(x.size() != y.size()) return false;                 // broj parametara mora biti isti
                
        nTocaka = (uint) x.size();
        return true;
}


FitnessP AproxEvalOp::evaluate(IndividualP individual)
{
	FitnessP fitness = static_cast<FitnessP> (new FitnessMin);
	
	BinaryP a0 = boost::dynamic_pointer_cast<Binary::Binary> (individual->getGenotype(0));
	BinaryP a1 = boost::dynamic_pointer_cast<Binary::Binary> (individual->getGenotype(1));
	BinaryP ampli = boost::dynamic_pointer_cast<Binary::Binary> (individual->getGenotype(2));
	BinaryP frekv = boost::dynamic_pointer_cast<Binary::Binary> (individual->getGenotype(3));
	BinaryP pomak = boost::dynamic_pointer_cast<Binary::Binary> (individual->getGenotype(4));
	
	uint i,j;
	double rez, suma = 0;
	for(i = 0; i < nTocaka; i++) { // FUNKCIJA CILJA
		rez = a0->realValue[0] + a1->realValue[0] * x[i];
		for(j=0;j<ampli->realValue.size();j++) rez += ampli->realValue[j] * sin( frekv->realValue[j] * x[i] + pomak->realValue[j] );
		suma += pow(rez - y[i], 2);
	} // REZULTAT JE SUMA KVADRATA ODSTUPANJA
	fitness->setValue(suma);
	
	return fitness;
}
