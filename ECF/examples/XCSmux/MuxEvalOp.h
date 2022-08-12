#ifndef MuxEvalOp_h
#define MuxEvalOp_h

#include "../../Individual.h"
#include "../../EvaluateOp.h"
#include "../../FitnessMax.h"

//TODO: razdvojit u poseban file

//Environment for MP problem (XCS example for single-step problem)
class MuxEvalOp : public Environment {

public:

	MuxEvalOp(StateP s) {state = s;};
	GenotypeP getInput();
	FitnessP MuxEvalOp::evaluate (IndividualP ind);
	bool initialize() {return true;};
	bool reset() {return true;};
};
typedef boost::shared_ptr<MuxEvalOp> MuxEvalOpP;

GenotypeP MuxEvalOp::getInput(){
	BitStringP input = static_cast<BitStringP> (new BitString::BitString);
	input->setGenotypeId(0);	// moramo postaviti indeks genotipa (prvi genotip)
	input->initialize(state);	// dobijemo slucajni niz bitova
	GenotypeP ret = input;
	return ret;
}
FitnessP MuxEvalOp::evaluate (IndividualP ind){
	// ovdje se racuna nagrada (reward) sustava
	// za mux problem, to znaci usporediti ulaz i odziv (akciju) sustava
	
	double reward = 0;
	BitStringP input = boost::dynamic_pointer_cast<BitString::BitString> (ind->getGenotype(0));
	BitStringP output = boost::dynamic_pointer_cast<BitString::BitString> (ind->getGenotype(1));

	// dekodiramo ulaz
	// za sada, hard-coded gledamo prvih 6 bitova (4/1 mux)
	uint address = 2;	// offset na 3. bitu
	uint weight = 1;
	for(uint adrBit = 2; adrBit >= 1; adrBit--) {
		if(input->bits[adrBit-1] == true)
			address += weight;
		weight *= 2;
	}
	
	// usporedba izlaza i akcije, davanje nagrade
	bool muxOut = input->bits[address];
	
	if(muxOut == output->bits[0])
		reward = 1000;
	else
		reward = 0;

	FitnessP fitness = static_cast<FitnessP> (new FitnessMax);
	fitness->setValue(reward);
	return fitness;

}

#endif // MuxEvalOp_h
