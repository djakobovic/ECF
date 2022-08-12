#include "SineFunction.h"
#include <math.h>

void SineFunction::applyActivationFunction(std::vector<double> *neurons) {
	for (size_t i = 0, n = neurons->size(); i < n; i++) {
		neurons->at(i) = sineFunction(neurons->at(i));
	}
}

void SineFunction::applyActivationFunctionDerivation(std::vector<double> *neurons) {
	for (size_t i = 0, n = neurons->size(); i < n; i++) {
		neurons->at(i) = cosineFunction(neurons->at(i));
	}
}

double SineFunction::cosineFunction(double x) {
	return cos(x);
}

double SineFunction::sineFunction(double x) {
	return sin(x);
}

SineFunction::SineFunction(std::string name) {
	this->name = name;
}