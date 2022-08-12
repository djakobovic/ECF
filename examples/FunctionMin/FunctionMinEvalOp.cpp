#include <ECF/ECF.h>
#include "FunctionMinEvalOp.h"


void FunctionMinEvalOp::registerParameters(StateP state)
{
	state->getRegistry()->registerEntry("function", (voidP) (new uint(1)), ECF::UINT);
}


bool FunctionMinEvalOp::initialize(StateP state)
{
	voidP sptr = state->getRegistry()->getEntry("function"); // get parameter value
	iFunction_ = *((uint*) sptr.get()); // convert from voidP to user defined type

	return true;
}


FitnessP FunctionMinEvalOp::evaluate(IndividualP individual)
{
	// evaluation creates a new fitness object using a smart pointer
	// in our case, we try to minimize the function value, so we use FitnessMin fitness (for minimization problems)
	FitnessP fitness (new FitnessMin);

	// we define FloatingPoint as the only genotype (in the configuration file)
	FloatingPoint::FloatingPoint* gen = (FloatingPoint::FloatingPoint*) individual->getGenotype().get();
	// (you can also use boost smart pointers:)
	//FloatingPointP gen = boost::dynamic_pointer_cast<FloatingPoint::FloatingPoint> (individual->getGenotype());

	// alternative encoding: Binary Genotype
	//Binary::Binary* gen = (Binary::Binary*) individual->getGenotype().get();
	//BinaryP gen = boost::dynamic_pointer_cast<Binary::Binary> (individual->getGenotype());

	// we implement the fitness function 'as is', without any translation
	// the number of variables is read from the genotype itself (size of 'realValue' vactor)
	double realTemp = 0, value = 0;

	switch(iFunction_) {
	case 1:
		for (uint i = 0; i < gen->realValue.size(); i++){
			realTemp = pow((gen->realValue[i] - (i + 1)), 2.);
			value += realTemp;
		}
	break;

	// Schaffer's F6 function
	case 2: {
		double z = 0;
		for(uint i = 0; i < gen->realValue.size(); ++i) {
			z += (gen->realValue[i] * gen->realValue[i]);
		}
		value = 0.5 - (pow(sin(sqrt(z)), 2) - 0.5) / pow(1 + 0.001 * z, 2);
		value = -1 * value + 1;
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
			realSum = pow(gen->realValue[i], 2.);
			valueSum += realSum;
			realCos = cos (2 * pi * gen->realValue[i]);
			valueCos += realCos;
		}
		value = -20 * exp(-0.2*sqrt(valueSum / gen->realValue.size())) - exp(valueCos / gen->realValue.size()) + 20 + exp(1.);
	} break;

	// Rastrigin
	case 5: {
		double realTemp = 0, pi = 3.141592;
		for (uint i = 0; i < gen->realValue.size(); i++) {
			realTemp = pow(gen->realValue[i], 2.) - 10 * cos(2 * pi * gen->realValue[i]);
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

	// Schaffer's F7 function
	case 7: {
		double z = 0;
		for(uint i = 0; i < gen->realValue.size(); ++i) {
			z += (gen->realValue[i] * gen->realValue[i]);
		}
		value = pow(z, 0.25) * (1.0 + pow( sin( 50 * pow(z, 0.1)), 2));
	} break;

	case 10: {
		double x1 = gen->realValue[0];
		double x2 = gen->realValue[1];
		value = pow(x1, 4)/4 - x1*x1 + 2*x1 + (x2 - 1)*(x2 - 1);
	} break;

	/// simulacija NN
	case 20: {
		double x10 = gen->realValue[0];
		double x11 = gen->realValue[1];
		double x20 = gen->realValue[2];
		double x21 = gen->realValue[3];
		double x30 = gen->realValue[4];
		double x31 = gen->realValue[5];
		double x32 = gen->realValue[6];
		double error = 0;
		for(uint i = 1; i <= 10; i++) {
			double net1, net2;
			//net1 = x10 + i * x11;
			net1 = 1. / (1 + exp(-(x10 + i * x11)));

			//net2 = x20 + i * x21;
			net2 = 1. / (1 + exp(-(x20 + i * x21)));

			net1 = x30 + net1 * x31 + net2 * x32;

			//error += pow(fabs(net1 - (200 + i/2.)), 2);
			error += pow(fabs(net1 - (i*i)), 2);
		}
		value = sqrt(error);
	} break;


	default:
		throw("FunctionMinEvalOp: invalid function index in configuration!");
	}

	fitness->setValue(value);
	return fitness;
}
