#include <math.h>
#include "SigmoidFunction.h"


void SigmoidFunction::applyActivationFunction(std::vector<double> *neurons) 
{
	/*Run trough all neurons in the layer and apply the sigmoid function to the output of the each neuron.
	In other words, activate the output.*/
	for (size_t i = 0, n = neurons->size(); i < n; i++)
		neurons->at(i) = SigmoidFunction::sigmoidFunction(neurons->at(i));
}


void SigmoidFunction::applyActivationFunctionDerivation(std::vector<double>* neurons) 
{
	/*Run trough all neurons in the layer and apply the sigmoid function derivation to the output of the each neuron.*/
	for (size_t i = 0, n = neurons->size(); i < n; i++)
		neurons->at(i) = SigmoidFunction::sigmoidFunctionDerivative(neurons->at(i));
}


double SigmoidFunction::sigmoidFunctionDerivative(double x) 
{
	return exp(x) / ((exp(x) + 1) * (exp(x) + 1));
}


double SigmoidFunction::sigmoidFunction(double x) 
{
	return 1. / (1 + exp(-x));
}


SigmoidFunction::SigmoidFunction(std::string name) 
{
	this->name = name;
}
