#ifndef SIGMOIDFUNCTION
#define SIGMOIDFUNCTION

#include "AbstractActivationFunction.h"

/*Classic sigmoid function used in most neural networks.*/
class SigmoidFunction : public AbstractActivationFunction {
public:
	void applyActivationFunction(std::vector<double> *neurons);

	void applyActivationFunctionDerivation(std::vector<double> *neurons);

	SigmoidFunction(std::string name);

private:
	/*Applies sigmoid activation function to the given floating point number.*/
	double static sigmoidFunction(double x);

	double sigmoidFunctionDerivative(double x);

};
#endif
