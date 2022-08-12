#ifndef NEURALNETWORK_H_
#define NEURALNETWORK_H_

#include "ActivationFunctions/AbstractActivationFunction.h"
#include "ActivationFunctions/LinearFunction.h"
#include "ActivationFunctions/SigmoidFunction.h"
#include "ActivationFunctions/SineFunction.h"

#include "Crossovers/CrsLayer.h"
#include "Crossovers/CrsNeuron.h"
#include "Crossovers/CrsNeuronUniform.h"
#include "Mutations/MutNeuron.h"
#include "Mutations/MutNeuronN.h"
#include "Mutations/MutScale.h"
#include "Mutations/MutScaleNeuron.h"
#include "Mutations/MutGauss.h"


/**
 * \class	NeuralNetwork
 *
 * \brief	A customized floating point genotype used for neural network implementation.
 *
 * \date	13.1.2017.
 */
class NeuralNetwork : public FloatingPoint::FloatingPoint {
public:

	/**
	 * \fn	NeuralNetwork::NeuralNetwork();
	 *
	 * \brief	Default constructor. Only sets the name of the genotype.
	 *
	 * \author	Rudolf Lovrencic
	 */
	NeuralNetwork();

	/**
	 * \fn	NeuralNetwork::NeuralNetwork(StateP state, string configFilePath);
	 *
	 * \brief	A special constructor for configuring the neural network from a config file.
	 * 			State is not necceceraly valid, just need a dummy for error logging and stuff.
	 *
	 *\author	Juraj Fulir
	 *
	 * \param	state		  	The state smart pointer.
	 * \param	configFilePath	Full pathname of the configuration file.
	 */
	NeuralNetwork(StateP state, string configFilePath);

	/**
	 * \fn	NeuralNetwork* NeuralNetwork::copy();
	 *
	 * \brief	Returns a copy of this genotype.
	 *
	 * \author	Juraj Fulir
	 *
	 * \return	Pointer to an identical NeuralNetwork object.
	 */
	NeuralNetwork* copy();

	/**
	 * \fn	void NeuralNetwork::registerParameters(StateP state);
	 *
	 * \brief	Registers the parameters required by the ECF.
	 *
	 * \author	Juraj Fulir
	 *
	 * \param	state	The state of the ECF system.
	 */
	void registerParameters(StateP state);

	/**
	 * \fn	std::vector<MutationOpP> NeuralNetwork::getMutationOp();
	 *
	 * \brief	Gets mutation operators in a vector.
	 *
	 * \author	Ivona Skorjanc
	 *
	 * \return	The vector filled with mutation operators.
	 */
	std::vector<MutationOpP> getMutationOp();

	/**
	 * \fn	std::vector<CrossoverOpP> NeuralNetwork::getCrossoverOp();
	 *
	 * \brief	Gets crossover operators in a vector.
	 *
	 * \author	Ivona Skorjanc
	 *
	 * \return	The vector filled with crossover operators.
	 */
	std::vector<CrossoverOpP> getCrossoverOp();

	/**
	 * \fn	bool NeuralNetwork::initialize(StateP state);
	 *
	 * \brief	Initializes the neural network. Sets all parameters required for default neural network operation.
	 *
	 * \author	Juraj Fulir, Kristijan Jaklinovic, Rudolf Lovrencic
	 *
	 * \param	state	The state of the ECF.
	 *
	 * \return	True if initialization was successful, false otherwise.
	 */
	bool initialize(StateP state);

	/**
	 * \fn	std::vector<uint> NeuralNetwork::getLayerStructure()
	 *
	 * \brief	Gets layers structure.
	 *
	 * \author	Rudolf Lovrencic
	 *
	 * \return	The number of neurons per layer.
	 */
	std::vector<uint> getLayerStructure();

	/**
	 * \fn	uint NeuralNetwork::getNoLayers()
	 *
	 * \brief	Getter for number of layers of the neural network.
	 *
	 * \author	Juraj Fulir
	 *
	 * \return	The number of layers.
	 */
	uint getNoLayers();

	/**
	 * \fn	uint NeuralNetwork::getNoInputs();
	 *
	 * \brief	Getter for number of inputs to the neural network.
	 *
	 * \author	Juraj Fulir
	 *
	 * \return	The number of inputs to the neural network.
	 */
	uint getNoInputs();

	/**
	 * \fn	uint NeuralNetwork::getNoOutputs()
	 *
	 * \brief	Getter for number of outputs from the neural network.
	 *
	 * \author	Juraj Fulir
	 *
	 * \return	The number of outputs from the neural network.
	 */
	uint getNoOutputs();

	/**
	 * \fn	void NeuralNetwork::sett(std::vector<double> inputs);
	 *
	 * \brief	Sets the current input. This function should be called before calculation of
	 * 			current outputs and be followed by calculateOutput function.
	 *
	 * \author	Rudolf Lovrencic
	 *
	 * \param	inputs	The current input to the network.
	 */
	void setCurrentInput(std::vector<double> inputs);

	/**
	 * \fn	void NeuralNetwork::getWeights(uint layer, uint neuron, vector<double>& weights);
	 *
	 * \brief	Gets the weights for a particular neuron, including the bias.
	 *
	 * \author	Juraj Fulir
	 *
	 * \param 		  	layer  	The layer in which the neuron is located.
	 * 							It ranges from 2 to 'n' because the first layer has no weights.
	 * \param 		  	neuron 	The index of that neuron in selected layer. It ranges from 1 to 'n'.
	 * \param [in,out]	weights	Reference to a vector filled with correct number of values for weights.
	 */
	void getWeights(uint layer, uint neuron, vector<double>& weights);

	/**
	 * \fn	uint NeuralNetwork::getNeuronPosition(uint layer, uint neuron);
	 *
	 * \brief	Gets the position of neuron's weights in a FP vector.
	 *
	 * \author	DJ
	 *
	 * \param 		  	layer  	The layer in which the neuron is located.
	 * 							It ranges from 2 to 'n' because the first layer has no weights.
	 * \param 		  	neuron 	The index of that neuron in selected layer. It ranges from 1 to 'n'.
	 * \return	Start index of this neuron's weights in the FP vector.
	 */
	uint getNeuronPosition(uint layer, uint neuron);

	/**
	 * \fn	void NeuralNetwork::setWeights(uint layer, uint neuron, vector<double>& weights);
	 *
	 * \brief	Sets the weights for a particular neuron, including the bias.
	 * 			layer -> T
	 * 			neuron -> The index of that neuron in selected layer. (1 to n)
	 * 			weights -> Reference to a vector filled with correct number of values for weights.
	 *
	 * \author	Juraj Fulir
	 *
	 * \param 		  	layer  	The layer in which the neuron is located.
	 * 							It ranges from 2 to 'n' because the first layer has no weights.
	 * \param 		  	neuron 	The index of that neuron in selected layer. It ranges from 1 to 'n'.
	 * \param [in,out]	weights	Reference to a vector filled with correct number of values for weights.
	 */
	void setWeights(uint layer, uint neuron, vector<double>& weights);

	/**
	 * \fn	bool NeuralNetwork::readConfigFile(StateP state, std::string filePath);
	 *
	 * \brief	Reads the structure and weights bounds and values from a file.
	 *
	 * \author	Juraj Fulir
	 *
	 * \param	state   	The state of the ECF system.
	 * \param	filePath	Full pathname of the file.
	 *
	 * \return	True if it succeeds, false if it fails.
	 */
	bool readConfigFile(StateP state, std::string filePath);

	/**
	 * \fn	bool NeuralNetwork::generateConfigFile(std::string filePath);
	 *
	 * \brief	Saves the structure and weight bounds and values to a file.
	 *
	 * \author	Juraj Fulir
	 *
	 * \param	filePath	Full pathname of the file.
	 *
	 * \return	True if it succeeds, false if it fails.
	 */
	bool generateConfigFile(std::string filePath);

	/**
	 * \fn	bool NeuralNetwork::readDataFile(std::string filePath);
	 *
	 * \brief	Reads the data file for required data (inputs and outputs) for calculations.
	 * 			Data is stored in inputData_ and outputData_ vectors.
	 *
	 * \author	Juraj Fulir
	 *
	 * \param	filePath	Full pathname of the file.
	 *
	 * \return	True if it succeeds, false if it fails. Note that exceptions may be thrown as well.
	 */
	bool readDataFile(std::string filePath);

	/**
	 * \fn	double NeuralNetwork::getTotalError();
	 *
	 * \brief	Calculates the fitness of this neural network.
	 *
	 * \author	Juraj Fulir
	 *
	 * \return	The total error.
	 */
	double getTotalError();

	/**
	 * \fn	vector<double>& NeuralNetwork::getOutput();
	 *
	 * \brief	Getter for the current output.
	 *
	 * \author	Rudolf Lovrencic, Kristijan Jaklinovic
	 *
	 * \return	The output.
	 */
	vector<double>& getOutput();

	/**
	 * \fn	void NeuralNetwork::setInputs(std::vector<double> inputData);
	 *
	 * \brief	Adds the input entry to the network. Adds an vector entry into a vector of input vectors.
	 *
	 * \author	Rudolf Lovrencic, Kristijan Jaklinovic
	 *
	 * \param	inputData	One input vector to the neural network.
	 */
	void addInputs(std::vector<double> inputData);

	/**
	 * \fn	void NeuralNetwork::calculateOutputs();
	 *
	 * \brief	Calculates outputs of the neural network and pushes them to the calculatedOutputs
	 * 			vector. Number of calculated outputs is equal to the number of lines in the data
	 * 			file. In other words it is equal to the amount of examples provided for neural
	 * 			network training.
	 *
	 * \author	Rudolf Lovrencic
	 */
	void calculateOutputs();

	/**
	 * \fn	std::vector<double> NeuralNetwork::calculateOutput();
	 *
	 * \brief	Calculates the output of the neural network based on the currentInput_ vector.
	 *
	 * \author	Rudolf Lovrencic
	 *
	 * \return	The calculated output.
	 */
	void calculateOutput(std::vector<double>&);
	
	std::vector<double> calculateOutput()
	{
		std::vector<double> output;
		calculateOutput(output);
		return output;
	}


	/**
	 * \fn	void NeuralNetwork::setActivationFunction(AbstractActivationFunction *activationFunction);
	 *
	 * \brief	Registers new activation function with the corresponding symbols. If it's name is found in the parameters file,
	 * 			this activation function will be used.
	 *
	 * \author	Rudolf Lovrencic, Kristijan Jaklinovic
	 *
	 * \param [in,out]	activationFunction	Activation function object that will be registered and can be used afterwards.
	 */
	void setActivationFunction(AbstractActivationFunction *activationFunction);

	/* Perform a single iteration of the backpropagation algorithm on the neural network over all data points. */
	void backpropagate(double learningRate);

	/** \brief	Input data read from the data file, if the file has been given. */
	std::vector<std::vector<double>> inputData_;

	/** \brief	Output data read from the data file, if the file has been given. These are expected outputs
				of the network used for neural network training.*/
	std::vector<std::vector<double>> outputData_;


private:
	/** \brief	Number of neurons per layer. */
	std::vector<uint> npLayer_;

	/** \brief	Max number of neurons per layer. */
	uint maxLayerSize_;

	/** \brief	Tag for deciding which error function to use. */
	std::string errorFunc_;

	/** \brief	Weights for each input data line. */
	std::vector<double> errorWeights_;

	// Input sum data for each data input, in each layer, for each neuron.
	std::vector<std::vector<std::vector<double>>> neuronSums_;
	// Output value for each data input, in each layer, from each neuron.
	std::vector<std::vector<std::vector<double>>> neuronOuts_;

	/*True if file has been provided by the user, otherwise false.*/
	bool dataFileExists;

	/** \brief	The current input to the network. */
	std::vector<double> currentInput_;

	/** \brief	True if file has been provided by the user, false otherwise.  */
	bool dataFileExists_;

	/** \brief	True if the file has been read at least once. False if the file still hasn't been read.*/
	bool dataFileRead_;

	/** \brief	Activation functions mapped by layers. This way we can fetch activation function for each NN layer without comparing strings.
				Note that number of layers starts from 1. In other words, layer mapped with 1 is the index of the first hidden layer.*/
	map<int, AbstractActivationFunction*> activationFunctionsPerLayers_;

	/** \brief	Activation function mapped with the character that represents that activation function in the parameters file.*/
	map<string, AbstractActivationFunction*> activationFunctionMapping_;

	/** \brief	Calculated outputs from the network. This should be compared with the expected output for error calculation.*/
	std::vector<std::vector<double>> calculatedOutputs_;

	/**
	 * \fn	uint NeuralNetwork::parseStructure(std::string structure);
	 *
	 * \brief	Parses the string that defines the structure of the neural network.
	 * 			Fills the npLayer_ vector with parsed values of given string.
	 *
	 * \author	Juraj Fulir
	 *
	 * \param	structure	The structure string read from the appropriate tag in the parameters file.
	 *
	 * \return	The dimension of needed FloatingPoint to store all weights and biases
	 */
	uint parseStructure(std::string structure);

	/**
	 * \fn	void NeuralNetwork::readErrorWeights(std::string filePath);
	 *
	 * \brief	Reads the error weights for each line of the file.
	 *
	 * \author	Juraj Fulir
	 *
	 * \param	filePath	Full pathname of the file containing error weights.
	 */
	void readErrorWeights(std::string filePath);

// <<<<<<< HEAD
//
// 	/*Calculated outputs from the network. This should be compatred with the expexted output for error calculation.*/
// 	std::vector<std::vector<double>> calculatedOutputs;
//
// 	/*Applies activation function to the provided vector of real numbers.
// 	In other words, the function applies the activation fucntion to the layer determined by the index.*/
// 	void applyActivationFunctionToTheLayer(std::vector<double> *unactivatedOutput, int layer);
//
// 	void applyActivationFunctionDerivativeToTheLayer(std::vector<double> &activatedOutput, int layer);
//
// 	/*Parsing a line that specifies activation functions per layers. Adds the objects to the activationFunctionsPerLayer map.*/
// 	void parseActivationsPerLayers(string functionsInAString);
//
// =======

	void printVector(vector<double>& vect);

	/**
	 * \fn	void NeuralNetwork::applyActivationFunctionToTheLayer(std::vector<double> *unactivatedOutput, int layer);
	 *
	 * \brief	Applies activation function to the provided vector of real numbers.
	 * 			In other words, the function applies the activation function to the layer determined by the index.
	 *
	 * \author	Rudolf Lovrencic, Kristijan Jaklinovic
	 *
	 * \param [in,out]	unactivatedOutput	The unactivated output that will become activated.
	 * \param 		  	layer			 	Index of a layer to apply the activation function.
	 */
	void applyActivationFunctionToTheLayer(std::vector<double> *unactivatedOutput, int layer);

	/**
	 * \fn	void NeuralNetwork::parseActivationsPerLayers(string functionsInAString);
	 *
	 * \brief	Parsing a line that specifies activation functions per layers. Adds the objects to the
	 * 			activationFunctionsPerLayer map.
	 *
	 * \author	Rudolf Lovrencic, Kristijan Jaklinovic
	 *
	 * \param	functionsInAString	The functions in a string.
	 */
	void parseActivationsPerLayers(string functionsInAString);

	/**
	 * \fn	void NeuralNetwork::applyActivationFunctionDerivativeToTheLayer(std::vector<double> &activatedOutput, int layer);
	 *
	 * \brief	Applies activation function derivative to the provided vector of real numbers.
	 * 			In other words, the function applies the activation function derivative to the layer determined by the index.
	 *
	 * \author	Juraj Fulir
	 *
	 * \param [in,out]	activatedOutput	The activated output that will become unactivated.
	 * \param 		  	layer			 	Index of a layer to apply the activation function derivative.
	 */
	 void applyActivationFunctionDerivativeToTheLayer(std::vector<double> &activatedOutput, int layer);
};

typedef boost::shared_ptr<NeuralNetwork> NeuralNetworkP;

#endif
