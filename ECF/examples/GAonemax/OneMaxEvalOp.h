#ifndef OneMaxEvalOp_h
#define OneMaxEvalOp_h

#include "../../Individual.h"
#include "../../EvaluateOp.h"
#include "../../FitnessMax.h"

class OneMaxEvalOp : public EvaluateOp
{
public:
// metoda za evaluaciju
	FitnessP evaluate(IndividualP individual);

};
//typedef boost::shared_ptr<OneMaxEvalOp> OneMaxEvalOpP;

// ovo inace ide u posebni .cpp fajl, ali dobro...
FitnessP OneMaxEvalOp::evaluate(IndividualP individual)
{
	// ovdje ide evaluacija jedinke
	// za ONEMAX problem, to znaci pobrojati broj jedinica u kromosomu

	// stvaranje zeljenog Fintess objekta:
	FitnessP fitness (new FitnessMax);
	
	// dohvat genotipa jedinke
	//BitStringP bitstr = boost::dynamic_pointer_cast<BitString::BitString> (individual->getGenotype(0));
	//BitStringP bitstr = boost::static_pointer_cast<BitString::BitString> (individual->getGenotype(0));
	BitString::BitString* bitstr = (BitString::BitString*) individual->getGenotype().get();
	
	// brojanje bitova
	uint ones = 0;
	for(uint i = 0; i<bitstr->bits.size(); i++){
		if(bitstr->bits[i] == true)
			ones++ ;
	}
	fitness->setValue(ones);

	return fitness;
}
#endif // OneMaxEvalOp_h
