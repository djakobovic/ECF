#ifndef ABSTRACTACTIVATIONFUNCTION
#define ABSTRACTACTIVATIONFUNCTION

#include <vector>
#include <map>
#include <string>

/*Abstraction of an activation function. User-defined activation functions should extend this class.*/
class AbstractActivationFunction {

public:
	std::string name;

	/*Applies the activation function to the unactivated output vector.*/
	virtual void applyActivationFunction(std::vector<double> *neurons) = 0;

	/*Applies the activation function derivation to the unactivated output vector.*/
	virtual void applyActivationFunctionDerivation(std::vector<double> *neurons) = 0;
};


#endif
