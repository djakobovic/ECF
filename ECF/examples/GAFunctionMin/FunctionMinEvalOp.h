#ifndef FUNCTIONMINEVALOP_H_
#define FUNCTIONMINEVALOP_H_

#include <cmath>
#include <limits>
#include "../../Individual.h"
#include "../../EvaluateOp.h"
#include "../../FitnessMin.h"

class FunctionMinEvalOp : public EvaluateOp 
{
public:
	FitnessP evaluate(IndividualP individual)
	{
		FitnessP fitness = static_cast<FitnessP> (new FitnessMin);

		//FloatingPointP gen = boost::dynamic_pointer_cast<FloatingPoint::FloatingPoint> (individual->getGenotype());
		FloatingPoint::FloatingPoint* gen = static_cast<FloatingPoint::FloatingPoint*> (individual->getGenotype().get());
		//BinaryP gen = boost::dynamic_pointer_cast<Binary::Binary> (individual->getGenotype());
		//Binary::Binary* gen = (Binary::Binary*) individual->getGenotype().get();

		int problem = 1;
		double realTemp = 0, value = 0;

		switch(problem) {
	case 0:	// proba prikaza Booleove fje
	{
		int fja = 0;
		for(int v0 = -1; v0 < 2; v0 += 2)
			for(int v1 = -1; v1 < 2; v1 += 2) {
				double value = v0 * gen->realValue[0] + v1 * gen->realValue[1];
				if(value >= 0)
					fja += 1;
				fja *= 2;
			}
		value = abs(fja - 4);
	}
		break;

	case 1:
		//implementirana funkcija za koju se racunam minimum je ujedno i fitness funkcija
		//fitness = fitness + (realValue[i] - (i + 1))^2
		for (uint i = 0; i < gen->realValue.size(); i++){
			realTemp = pow((gen->realValue.at(i) - (i + 1)), 2.);

			//double x = 20 + 20 * pow(sin(gen->realValue[i]), 2);
			//double x = 8 + 20 / (1 + exp(-1.0 * gen->realValue[i]));
			//double x = 8 + 20 * 0.5 * (1 + gen->realValue[i] / sqrt(1 + pow(gen->realValue[i], 2)));
			//realTemp = pow((x - (i + 1)), 2.);
			value += realTemp;
		}

		break;

	// Schaffer's F6 function
	case 2: {
		double z = 0;
		for(uint i = 0; i < gen->realValue.size(); ++i) {
			z += (gen->realValue[i] * gen->realValue[i]);
		}
		value = 0.5 + (pow(sin(sqrt(z)), 2) - 0.5) / pow(1 + 0.001 * z, 2);
	} break;

	// Griewangk
	case 3: {
		double valueSum = 0, valueProduct = 1, realSum = 0, realProduct = 0;
		for (uint i = 0; i < gen->realValue.size(); i++){
			realSum = pow(gen->realValue[i], 2.)/4000;
			valueSum += realSum;
			realProduct = cos(gen->realValue[i] / sqrt((double)(i+1)));
			valueProduct *= realProduct;
		}
		value = valueSum - valueProduct + 1;
	} break;

	// Ackley
	case 4: {
		double realSum = 0, valueSum = 0, realCos = 0, valueCos = 0, pi = 3.141592;
		for (uint i = 0; i < gen->realValue.size(); i++){
			realSum = pow(gen->realValue[i],2.);
			valueSum += realSum;
			realCos = cos (2*pi*gen->realValue[i]);
			valueCos += realCos;
		}
		value = -20 * exp(-0.2*sqrt(valueSum / gen->realValue.size())) - exp(valueCos / gen->realValue.size()) + 20 + exp(1.);
	} break;

	// Rastrigin
	case 5: {
		double realTemp = 0, pi = 3.141592;
		for (uint i = 0; i < gen->realValue.size(); i++) {
			realTemp = pow(gen->realValue[i],2.) - 10 * cos(2*pi*gen->realValue[i]);
			value += realTemp;
		}
		value = value + 10 * gen->realValue.size();
	} break;

	// Rosenbrock
	case 6: {
		double realTemp = 0;
		for (uint i = 0; i < gen->realValue.size() - 1; i++){
			realTemp = 100 * pow(gen->realValue[i + 1] - pow(gen->realValue[i], 2.), 2.) + pow(1 - gen->realValue[i], 2.);
			value += realTemp;
		}
	} break;

		} // switch

		fitness->setValue(value);
		return fitness;
	}
};
typedef boost::shared_ptr<FunctionMinEvalOp> FunctionMinEvalOpP;

#endif /* FUNCTIONMINEVALOP_H_ */
