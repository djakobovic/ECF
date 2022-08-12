#ifndef SINUSFUNCTION
#define SINUSFUNCTION

#include "AbstractActivationFunction.h"

/*Classic sigmoid function used in most neural networks.*/
class SineFunction : public AbstractActivationFunction {
public:
	void applyActivationFunction(std::vector<double> *neurons);

	void applyActivationFunctionDerivation(std::vector<double> *neurons);

	SineFunction(std::string name);

private:
	/*Applies sine activation function to the given floating point number.*/
	double sineFunction(double x);

	/*Applies the cosine activation function to the given floating point number.
	This is used for derivation of the activation function.*/
	double cosineFunction(double x);
};
#endif
