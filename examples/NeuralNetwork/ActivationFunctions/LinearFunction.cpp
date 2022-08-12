#include "LinearFunction.h"

void LinearFunction::applyActivationFunction(std::vector<double> *neurons) {
	/*Applying the linear function - "do nothing"!
	Simply forward the vector without changing it.*/
}

void LinearFunction::applyActivationFunctionDerivation(std::vector<double>* neurons) {
	for (size_t i = 0, n = neurons->size(); i < n; i++) {
		neurons->at(i) = 1;
	}
}

LinearFunction::LinearFunction(std::string name) {
	this->name = name;
}
