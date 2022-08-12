#include <ECF/ECF.h>
#include "FunctionMinEvalOp.h"
#include "NeuralNetwork.h"
#include "ActivationFunctions/SineFunction.h"
#include "Algorithms/Backpropagation.h"

int main(int argc, char **argv) {
	StateP state(new State);

	/*Setting the evaluation operator.*/
	FunctionMinEvalOpP func (new FunctionMinEvalOp);
	state->setEvalOp(func);

	/*Setting the backpropagation as the working algorithm.*/
	BackpropagationP bp(new Backpropagation);
	state->addAlgorithm(bp);

	/*Adding a NeuralNetwork genotype.*/
	NeuralNetworkP nn(new NeuralNetwork);
	state->addGenotype(nn);

	/*User provided activation function.*/
	SineFunction* sinusTest = new SineFunction("sin");
	nn->setActivationFunction(sinusTest);

	state->initialize(argc, argv);
	state->run();

	NeuralNetwork* f = (NeuralNetwork*)state->getHoF()->getBest()[0]->getGenotype().get();
	f->generateConfigFile("Testing/configFile");

	// PROGRAM FOR TESTING THE RESULT NN

	// cout << "\n>>>Time to use this program!<<<\n";
	// while (true) {
	// 	std::vector<double> inputs;
	// 	double value;
	// 	for (uint i = 0; i < f->getNoInputs(); i++) {
	// 		cout << "\nEnter " << i << ". argument(input): ";
	// 		cin >> value;
	// 		inputs.push_back(value);
	// 	}
	//
	// 	f->setCurrentInput(inputs);
	//
	// 	std::vector<double> learnedOutput = f->calculateOutput();
	//
	// 	cout << "\nOutput: ";
	// 	for (uint i = 0, n = f->getNoOutputs(); i < n; i++)
	// 		cout << "[" << learnedOutput[i] << "]";
	// cout << "\nEnded.";


	ofstream file("Testing/resultsData.txt");
	if (file.is_open()) {
		if(f->getNoOutputs() == 1)
		{
			file << "Expected Result\n";
			for (uint dat = 0; dat < f->inputData_.size(); dat++){
				f->setCurrentInput(f->inputData_[dat]);
				vector<double> results = f->calculateOutput();
				file << f->outputData_[dat][0]  << " " << results[0]  << "\n";
			}
		}
		else
		{
			// Init titles.
			for(uint i=1; i<=f->getNoOutputs(); i++)
				if(i==f->getNoOutputs())
					file << "Expected"<<i<<" Result"<<i<<"\n";
				else
					file << "Expected"<<i<<" Result"<<i<<" ";

			for (uint dat = 0; dat < f->inputData_.size(); dat++)
			{
				for(uint i=0; i<f->getNoOutputs(); i++)
				{
					f->setCurrentInput(f->inputData_[dat]);
					vector<double> results = f->calculateOutput();
					if(i==f->getNoOutputs()-1)
						file << f->outputData_[dat][i]  << " " << results[i] << "\n";
					else
						file << f->outputData_[dat][i]  << " " << results[i] << " ";
				}
			}
		}
	}
	else {
		cout << "Unable to open data file";
	}
	file.close();


	ofstream fileErr("Testing/resultsError.txt");
	if (fileErr.is_open()) {
		fileErr << "TotalError\n";
		for (uint dat = 0; dat < func->Errors_.size(); dat++){
			fileErr << func->Errors_.at(dat) << "\n";
		}
	}
	else {
		cout << "Unable to open error file";
	}
	fileErr.close();

	return 0;
}
