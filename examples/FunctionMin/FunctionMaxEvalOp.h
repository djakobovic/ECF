#ifndef FUNCTIONMAXEVALOP_H_
#define FUNCTIONMAXEVALOP_H_

#include <cmath>
#include "Individual.h"
#include "EvaluateOp.h"
#include "FitnessMax.h"

class FunctionMaxEvalOp: public EvaluateOp {
public:
	FitnessP evaluate(IndividualP individual){

				FitnessP fitness = static_cast<FitnessP> (new FitnessMax);

				Binary* bin = (Binary*) individual->at(0).get();

				fitness->value_ = 0;
				float realTemp = 0;
				//implementirana funkcija za koju se racunam maksimum je ujedno i fitness funkcija
				//fitness = fitness - (realValue[i] - (i + 1))^2
				for (uint i = 0; i < bin->variables.size(); i++){
					realTemp = pow((bin->realValue.at(i) - (i + 1)), 2.);
					fitness->value_ = fitness->value_ - realTemp;
				}
				printf ("Vrijednost fitness funkcije: %f\n", fitness->value_);

				return fitness;
		}
};
typedef boost::shared_ptr<FunctionMaxEvalOp> FunctionMaxEvalOpP;

#endif /* FUNCTIONMAXEVALOP_H_ */
