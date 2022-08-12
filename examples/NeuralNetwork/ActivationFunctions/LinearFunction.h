#ifndef LINEARFUNCTION
#define LINEARFUNCTION

#include "AbstractActivationFunction.h"

/*Linear activation function. This function just forwads the provided vector to the next layer
or the neural network output.*/
class LinearFunction : public AbstractActivationFunction {
public:
	LinearFunction(std::string name);

	void applyActivationFunction(std::vector<double> *neurons);

	void applyActivationFunctionDerivation(std::vector<double> *neurons);
};

#endif
