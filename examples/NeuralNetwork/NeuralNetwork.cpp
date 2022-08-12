#include <ECF/ECF.h>
#include <math.h>
#include <string.h>

#include "NeuralNetwork.h"


NeuralNetwork::NeuralNetwork() 
{
	name_ = "NeuralNetwork";
}


NeuralNetwork* NeuralNetwork::copy() 
{
	NeuralNetwork *newObject = new NeuralNetwork(*this);
	return newObject;
}


uint NeuralNetwork::getNoLayers() 
{
	return npLayer_.size();
}


uint NeuralNetwork::getNoInputs() 
{
	return npLayer_[0];
}


uint NeuralNetwork::getNoOutputs() 
{
	return npLayer_[getNoLayers() - 1];
}


void NeuralNetwork::registerParameters(StateP state) 
{
	FloatingPoint::registerParameters(state);
	// XML will now support the following entries in addition to the old ones
	registerParameter(state, "structure", (voidP)(new string("2 3 1")), ECF::STRING, "number of neurons per layer (default: 2 3 1)");
	registerParameter(state, "outFunction", (voidP) new uint(1), ECF::UINT, "activation function of the output layer (default: 1)");
	registerParameter(state, "configFile", (voidP)(new string()), ECF::STRING, "path to file containing the NN structure and weight values");
	registerParameter(state, "dataFile", (voidP)(new string()), ECF::STRING, "path to file containing training data (mandatory)");
	registerParameter(state, "errorWeights", (voidP)(new string()), ECF::STRING, "path to file containing weights for every training data line.");
	registerParameter(state, "errorFunc", (voidP)(new string("RMSE")), ECF::STRING, "tag for error function used in getTotalError (default: RMSE)");
	registerParameter(state, "activations", (voidP)(new string("sig lin")), ECF::STRING, "Activation functions per layer. Input layer has no activation function.");
}


std::vector<MutationOpP> NeuralNetwork::getMutationOp() 
{
	std::vector<MutationOpP> mut(FloatingPoint::getMutationOp());
	mut.push_back(static_cast<MutationOpP> (new MutNeuron));
	mut.push_back(static_cast<MutationOpP> (new MutNeuronN));
	mut.push_back(static_cast<MutationOpP> (new MutScale));
	mut.push_back(static_cast<MutationOpP> (new MutScaleNeuron));
	mut.push_back(static_cast<MutationOpP> (new MutGauss));
	
	return mut;
}


std::vector<CrossoverOpP> NeuralNetwork::getCrossoverOp() 
{
	std::vector<CrossoverOpP> crx(FloatingPoint::getCrossoverOp());

	crx.push_back(static_cast<CrossoverOpP> (new CrsLayer));
	crx.push_back(static_cast<CrossoverOpP> (new CrsNeuron));
	crx.push_back(static_cast<CrossoverOpP> (new CrsNeuronUniform));

	return crx;
}


NeuralNetwork::NeuralNetwork(StateP state, string configFilePath) 
{
	readConfigFile(state, configFilePath);
}


bool NeuralNetwork::initialize(StateP state) 
{
	dataFileRead_ = false;

	/*Creating with new operator because traditional way makes object disappears after losing scope.*/
	LinearFunction* lin = new LinearFunction("lin");
	SigmoidFunction* sig = new SigmoidFunction("sig");
	/*Adding two main functions to the activation function roster (activationFunctionMapping).*/
	setActivationFunction(lin);
	setActivationFunction(sig);

	// GET parameters values
	voidP sptr = getParameterValue(state, "structure");
	string structure = *((string*)sptr.get());
	parseStructure(structure);

	if (isParameterDefined(state, "activations")) {
		/*Fetching the activations per layer from the parameters file.*/
		voidP sptr = getParameterValue(state, "activations");
		string activations = *((string*)sptr.get());
		parseActivationsPerLayers(activations);
		/*Clears a number of neurons per layer vector since this function is called later. We needed to call this
		function before since parseActivationsPerLayers() needs the number of layers to be working properly.*/
		npLayer_.clear();
	}

	sptr = getParameterValue(state, "configFile");
	string config = *((string*)sptr.get());

	if (config.empty())
	{
		// VALIDATE parameters definition
		if (structure.empty())
		{
			ECF_LOG_ERROR(state, "Error: structure for NN must be set (format: 2-3-1)!");
			throw("");
		}

		uint dimensionsNum = parseStructure(structure);

		if (npLayer_.size() < 2)
		{
			ECF_LOG_ERROR(state, "Error: NN structure must have at least 2 layers (input-output)!");
			throw("");
		}

		// Set the vector dimension and initialize the FloatingPoint parent
		Genotype::setParameterValue(state, std::string("dimension"), (voidP)(new uint(dimensionsNum)));

		FloatingPoint::initialize(state);
	}
	else
	{
		readConfigFile(state, config);
	}

	dataFileExists_ = false;

	// Training data and parameters.
	if (isParameterDefined(state, "dataFile")) {
		dataFileExists_ = true;
		sptr = getParameterValue(state, "errorFunc");
		errorFunc_ = *((string*)sptr.get());

		sptr = getParameterValue(state, "dataFile");

		if (!dataFileRead_) {
			readDataFile(*((string*)sptr.get()));
			dataFileRead_ = true;
		}
		if (isParameterDefined(state, "errorWeights")) {
			sptr = getParameterValue(state, "errorWeights");
			readErrorWeights(*((string*)sptr.get()));
		}
		else {
			for (uint i = 0; i < inputData_.size(); i++)
				errorWeights_.push_back(1);
		}
	}


	// cout<<"\nReal values:";
	// printVector(realValue);
	// cout<<"\n";

	return true;
}


uint NeuralNetwork::parseStructure(std::string structure) 
{
	// Calculates the needed dimensions
	uint dimensionsNum = 0;

	// Clear the vector to fix multiple call overfilling error.
	npLayer_.clear();

	// Read the structure (number of neurons per layer).
	maxLayerSize_ = 0;
	string buffer = "";
	for (size_t i = 0; i < structure.length() + 1; i++) {
		if ((i == structure.length() || isspace(structure[i])) && !buffer.empty()) {
			uint value = stoi(buffer);
			if (value <= 0) {
				throw std::invalid_argument("Error: number of neurons per layer must be > 0!");
			}
			npLayer_.push_back(value);

			// update max layer size
			if(value > maxLayerSize_)
				maxLayerSize_ = value;

			// Reset buffer
			buffer = "";

			// Input layer has no weights
			if (npLayer_.size() > 1) {
				// Multiply the amount of neurons in last layer (+ 1 bias) with the amount of neurons in
				// this layer
				dimensionsNum += (1 + npLayer_[npLayer_.size() - 2]) * npLayer_[npLayer_.size() - 1];
			}
		}
		else
			buffer += structure[i];
	}
	return dimensionsNum;
}


// Gets the weights for a parcitular neuron, including the bias.
// layer -> The layer in which the neuron is located. (2 to n because the
//          1st layer has no weights.)
// neuron -> The index of that neuron in selected layer. (1 to n)
// weights -> Pointer to a vector to be filled with values of weights
void NeuralNetwork::getWeights(uint layer, uint neuron, vector<double>& weights) 
{
	if (layer < 2 || layer > getNoLayers()) {
		//    throw std::invalid_argument("Error: Cannot read weights, invalid layer index. Given: "+std::to_string(layer));
		throw std::invalid_argument("Error: Cannot read weights, invalid layer index. Given: " + uint2str(layer));
	}
	if (neuron < 1 || neuron > npLayer_[layer - 1]) {
		throw std::invalid_argument("Error: Cannot read weights, invalid neuron index. Given: " + uint2str(neuron));
	}

	// Correct the indexes (inputs are indexed with 1..n, arrays are indexed with 0..n-1)
	layer--;
	neuron--;

	uint position = 0; // FP pointer

	uint l = 0;
	for (l = 1; l < layer; l++)
		position += (1 + npLayer_[l - 1]) * npLayer_[l];
	position += (1 + npLayer_[l - 1]) * neuron;

	weights.resize(1 + npLayer_[l - 1]);
	for (uint i = 0; i < 1 + npLayer_[l - 1]; i++)
		weights[i] = realValue[position + i];
}


uint NeuralNetwork::getNeuronPosition(uint layer, uint neuron)
{
	// Correct the indexes (inputs are indexed with 1..n, arrays are indexed with 0..n-1)
	layer--;
	neuron--;

	uint position = 0; // FP pointer

	uint l = 0;
	for (l = 1; l < layer; l++)
		position += (1 + npLayer_[l - 1]) * npLayer_[l];
	position += (1 + npLayer_[l - 1]) * neuron;

	return position;
}


void NeuralNetwork::setWeights(uint layer, uint neuron, vector<double>& weights) 
{
	if (layer < 2 || layer > getNoLayers()) {
		throw std::invalid_argument("Error: Cannot read weights, invalid layer index. Given: " + uint2str(layer));
	}
	if (neuron < 1 || neuron > npLayer_[layer - 1]) {
		throw std::invalid_argument("Error: Cannot read weights, invalid neuron index. Given: " + uint2str(neuron));
	}
	if (weights.size() != 1 + npLayer_[layer - 2]) {
		throw std::invalid_argument("Error: Insufficient number of weights. Given: " + uint2str(weights.size()));
	}

	// Correct the indexes (inputs are indexed with 1..n, arrays are indexed with 0..n-1)
	layer--;
	neuron--;

	uint position = 0; // FP pointer

	uint l = 0;
	for (l = 1; l < layer; l++)
		position += (1 + npLayer_[l - 1]) * npLayer_[l];
	position += (1 + npLayer_[l - 1]) * neuron;

	for (uint i = 0; i < 1 + npLayer_[l - 1]; i++)
		realValue[position + i] = weights[i];
}


bool NeuralNetwork::readConfigFile(StateP state, string dataFilepath) 
{
	uint dimensions = 0;

	// Open input stream.
	ifstream file;
	file.open(dataFilepath);

	if (file.is_open()) {
		string line;
		// Read the structure
		if (getline(file, line) && !line.empty()) {
			dimensions = parseStructure(line);
		}

		if (getline(file, line) && !line.empty()) {
			string buffer = "";
			uint i = 0;
			// Read the lower bound.
			while (!isspace(line[i])) {
				buffer += line[i];
				i++;
			}
			// The weight bounds
			Genotype::setParameterValue(state, std::string("lbound"), (voidP)(new double(stod(buffer))));

			buffer = "";
			// Read the upper bound.
			for (uint j = i; j < line.length(); j++)
				buffer += line[j];
			Genotype::setParameterValue(state, std::string("ubound"), (voidP)(new double(stod(buffer))));
		}

		// Set the vector dimension and initialize the FloatingPoint parent
		Genotype::setParameterValue(state, std::string("dimension"), (voidP)(new uint(dimensions)));

		// For each layer read and store weights
		std::vector<double> values;
		if (getline(file, line) && !line.empty()) {
			// Parse the values from string.
			string buffer = "";
			// cout<<"\nLine: "<<line;
			for (uint i = 0; i < line.length() + 1; i++) {
				if ((i == line.length() || isspace(line[i])) && !buffer.empty()) {
					values.push_back(stod(buffer));
					// Reset buffer
					buffer = "";
				}
				else
					buffer += line[i];
			}
		}
		// cout<<"\nWeights:";
		// printVector(values);

		if (getline(file, line) && !line.empty())
			parseActivationsPerLayers(line);
		// cout<<"\nFunc: "<<line;

		if (dimensions != values.size())
			throw std::invalid_argument("Error: Insufficient number of weights. Given: " + uint2str(values.size()));

		FloatingPoint::initialize(state);

		realValue = values;

		file.close();
	}
	else {
		return false;
	}
	return true;
}


bool NeuralNetwork::generateConfigFile(std::string filePath) 
{
	ofstream file(filePath);
	if (file.is_open()) {
		// file << "This is a line.\n";
		for (uint i = 0; i < npLayer_.size(); i++)
			file << npLayer_[i] << " ";
		file << "\n" << getLBound() << " " << getUBound() << "\n";
		for (uint i = 0; i < realValue.size(); i++)
			file << realValue[i] << " ";
		file << "\n";
		for(uint i = 2; i <= getNoLayers(); i++)
			file << activationFunctionsPerLayers_.at(i)->name << " ";

		file.close();
	}
	else {
		cout << "Unable to open file";
		file.close();
		return false;
	}

	return true;
}


bool NeuralNetwork::readDataFile(std::string filePath) 
{
	// Open input stream.
	ifstream file;
	file.open(filePath);

	if (file.is_open()) {
		uint nInputs = getNoInputs();

		string line;
		while (getline(file, line)) {
			if (!line.empty()) {
				std::vector<double> inp;
				std::vector<double> out;

				// Parse the values from string.
				string buffer = "";
				for (uint i = 0; i < line.length() + 1; i++) {
					// Delimiter
					if (i == line.length() || isspace(line[i])) {
						double value = stod(buffer);

						if (inp.size() < nInputs) {
							inp.push_back(value);
						}
						else {
							out.push_back(value);
						}

						// Reset buffer
						buffer = "";
					}
					else
						buffer += line[i];
				}
				inputData_.push_back(inp);
				outputData_.push_back(out);
			}
		}
		file.close();

		if (inputData_.size() != outputData_.size()) {
			throw std::string("NeuralNetwork Error: number of inputs and outputs isn't equal");
		}
	}
	else {
		//throw std::invalid_argument("NeuralNetwork Error: unable to open data file: "+filePath);
		throw std::string("NeuralNetwork Error: unable to open data file: " + filePath);
		return false;
	}
	return true;
}


void NeuralNetwork::readErrorWeights(std::string filePath) 
{
	// Open input stream.
	ifstream file;
	file.open(filePath);

	if (file.is_open()) {
		double inputs_sum = 0;

		string line;
		// Parse the weights from file (one weight per line).
		while (getline(file, line)) {
			if (!line.empty()) {
				double value = stod(line);
				errorWeights_.push_back(value);
				inputs_sum += value;
			}
		}
		file.close();

		if (errorWeights_.size() < inputData_.size()) {
			throw std::invalid_argument("NeuralNetwork Error: errorWeights file hasn't got enough weights");
		}

		// Turn the weights into percentages.
		for (uint i = 0; i < errorWeights_.size(); i++)
			errorWeights_[i] = errorWeights_[i] / inputs_sum;
	}
	else {
		throw std::invalid_argument("NeuralNetwork Error: unable to open errorWeights file: " + filePath);
	}
}


double NeuralNetwork::getTotalError() 
{
	double totalError = 0;
	double* result;
	std::vector<double> output(getNoOutputs());

	// calculateOutputs();

	for (uint dataIt = 0; dataIt < inputData_.size(); dataIt++) {
		result = &(outputData_[dataIt][0]);
		setCurrentInput(inputData_[dataIt]);
		calculateOutput(output);

		double error = 0;
		if (errorFunc_.compare("MSE") == 0) {
			for (uint i = 0; i < getNoOutputs(); i++) {
				error += (output[i] - result[i])*(output[i] - result[i]);
			}
			error /= getNoOutputs();
		}

		else if (errorFunc_.compare("RMSE") == 0) {
			for (uint i = 0; i < getNoOutputs(); i++) {
				error += (output[i] - result[i])*(output[i] - result[i]);
			}
			error /= getNoOutputs();
			error = sqrt(error);
		}

		else if (errorFunc_.compare("MAE") == 0) {
			for (uint i = 0; i < getNoOutputs(); i++) {
				error += abs(output[i] - result[i]);
			}
			error /= getNoOutputs();
		}

		else {
			for (uint i = 0; i < getNoOutputs(); i++) {
				error += (output[i] - result[i])*(output[i] - result[i]);
			}
			error /= getNoOutputs();
			error = sqrt(error);
		}

		// Weight the importance of this error.
		totalError += error * errorWeights_[dataIt];
	}
	// totalError = sqrt(totalError);
	return totalError;
}


std::vector<uint> NeuralNetwork::getLayerStructure() 
{
	return npLayer_;
}


void NeuralNetwork::addInputs(std::vector<double> inputData) 
{
	if (dataFileExists_) {
		throw std::invalid_argument("Error: Cannot set input. Data file has been provided!");
	}
	this->inputData_.push_back(inputData);
}


void NeuralNetwork::setCurrentInput(std::vector<double> inputs) 
{
	currentInput_ = inputs;
}


void NeuralNetwork::calculateOutput(vector<double>& output) 
{
	// Store the inputs as outputs of the 1st layer.
	vector<double> lastOutputs(maxLayerSize_);
	vector<double> thisOutputs(maxLayerSize_); // Outputs for this layer.

	for(uint inp=0; inp < currentInput_.size(); inp++)
			lastOutputs[inp] = currentInput_[inp];
	//Offset from the beginning of the real value - calculated after moving to the next layer.
	//This allows for fetching the weights for new working layer.
	uint offset = 0;

	// Calculate rest of the neuron outputs.
	for (uint layer = 1; layer < getNoLayers(); layer++) {
		uint nLastOutputs = npLayer_[layer - 1]; // size of the previous layer

		for (uint neuron = 1; neuron <= npLayer_[layer]; neuron++) {

			double sum = realValue[offset]; // Bias value is just added.
			for (uint w = 1; w < nLastOutputs + 1; w++)
    			sum += realValue[offset + w] * lastOutputs[w - 1]; // Last layers outputs are this layers neuron inputs.
			offset += nLastOutputs + 1;	// increase offset for the next neuron

			thisOutputs[neuron - 1] = sum; // Stores all the sums for this layer.
		}
		applyActivationFunctionToTheLayer(&thisOutputs, layer + 1); // Turn those sums into outputs.

		// move to next layer
		lastOutputs = thisOutputs;
	}

	lastOutputs.resize(getNoOutputs());
	output = lastOutputs;
}


void NeuralNetwork::applyActivationFunctionToTheLayer(vector<double> *unactivatedOutput, int layer) 
{
	AbstractActivationFunction *aafForThisLayer = activationFunctionsPerLayers_[layer];

	aafForThisLayer->applyActivationFunction(unactivatedOutput);
}


void NeuralNetwork::applyActivationFunctionDerivativeToTheLayer(vector<double> &activatedOutput, int layer) 
{
  AbstractActivationFunction *aafForThisLayer = activationFunctionsPerLayers_.at(layer);

  aafForThisLayer->applyActivationFunctionDerivation(&activatedOutput);
}


void NeuralNetwork::setActivationFunction(AbstractActivationFunction *activationFunction) 
{
	activationFunctionMapping_.insert(std::pair<string, AbstractActivationFunction*>(activationFunction->name, activationFunction));
}


void NeuralNetwork::parseActivationsPerLayers(string functionsInAString) 
{
	vector<string> splittedLine;

	/*Splitting the line by the space and filling the vector.*/
	std::istringstream iss(functionsInAString);
	do {
		string sub;
		iss >> sub;
		splittedLine.push_back(sub);
	} while (iss);
	// splittedLine.pop_back(); // Čemu?

	/*If the parameter for the whole neural network has been set.*/
	if (splittedLine.size() == 1) {
		AbstractActivationFunction* f = activationFunctionMapping_.at(splittedLine[0]);

//<<<<<<< HEAD
		/*nedostaje provjera ako nema fje u mapi*/
		for (uint layer = 2, n = getNoLayers(); layer <= n; layer++) {
			activationFunctionsPerLayers_.insert(std::pair<int, AbstractActivationFunction*>(layer, f));
		}
	}
	else {
		for (uint layer = 2, n = splittedLine.size(); layer <= n; layer++) {
			/*nedostaje provjera ako element nije u mapi*/
			//vationFunctionMapping.at("dsfsd")
			AbstractActivationFunction* f = activationFunctionMapping_.at(splittedLine[layer-2]);
			activationFunctionsPerLayers_.insert(std::pair<int, AbstractActivationFunction*>(layer, f));
// =======
// 		/*TODO: check if the function is actually in the map*/
//
// 		for (uint i = 0, n = getNoLayers(); i < n; i++) {
// 			activationFunctionsPerLayers_.insert(std::pair<int, AbstractActivationFunction*>(i + 1, f));
// 		}
// 	}
// 	else {
// 		for (uint i = 0, n = splittedLine.size(); i < n; i++) {
//
// 			/*TODO: check if the function is actually in the map*/
//
// 			AbstractActivationFunction* f = activationFunctionMapping_.at(splittedLine[i]);
// 			activationFunctionsPerLayers_.insert(std::pair<int, AbstractActivationFunction*>(i + 1, f));
// >>>>>>> development
		}

		// // Fill the missing layer with the last mentioned function.
		// for(uint layer = splittedLine.size()+1; layer <= getNoLayers(); layer++) {
		// 	AbstractActivationFunction* f = activationFunctionMapping.at(splittedLine.back());
		// 	activationFunctionsPerLayers.insert(std::pair<int, AbstractActivationFunction*>(layer, f));
		// 	cout<<"Fill:"<<layer<<" "<<f->name<<"\n";
		// }
	}
}


void NeuralNetwork::calculateOutputs() 
{
	calculatedOutputs_.clear();
	calculatedOutputs_.resize(inputData_.size());

	for (size_t i = 0, n = inputData_.size(); i < n; i++) {
		currentInput_ = inputData_.at(i);
		calculateOutput(calculatedOutputs_[i]);
	}
}


vector<double>& NeuralNetwork::getOutput() {
	if (dataFileExists_) {
		throw std::invalid_argument("Error: Cannot return output. Input data file has been provided!");
	}

	return calculatedOutputs_.at(0);
}


/* Perform a single iteration of the backpropagation algorithm on the neural network over all data points. */
void NeuralNetwork::backpropagate(double learningRate)
{
// On-line training.
	for (uint dataIndex = 0; dataIndex < inputData_.size(); dataIndex++) {

// cout<<"\n=================================================";
// cout<<"\nDataIndex: " << dataIndex;
// cout<<"\n-------------------------------------------------";


// Derivative activation from sums of the each neuron, per layer (layers are incremental).
		vector<vector<double>> Derivatives;
// Output of the each neuron, per layer (layers are incremental).
		vector<vector<double>> Outputs;
// Error of the each neuron, per layer (layers are decremental).
		vector<vector<double>> Errors;


// Store the inputs as outputs of the 1st layer.
		vector<double> thisOutputs;

    for(uint inp=0; inp < inputData_[dataIndex].size(); inp++)
			thisOutputs.push_back( inputData_[dataIndex][inp] );

		Outputs.push_back( thisOutputs );


// Calculate rest of the neuron outputs.
		for (uint layer = 2; layer <= getNoLayers(); layer++) {
			vector<double> thisOutputs; // Outputs for this layer.

			for (uint neuron = 1; neuron <= npLayer_[layer-1]; neuron++) {

        vector<double> weights; // Weights of a single neuron in this layer.
        getWeights(layer, neuron, weights);

        double sum = weights[0]; // Bias value is just added.
    		for (uint w = 1; w < weights.size(); w++)
    			sum += weights[w] * Outputs.back()[w-1]; // Last layers outputs are this layers neuron inputs.

        thisOutputs.push_back( sum ); // Stores all the sums for this layer.
			}

			Derivatives.push_back( thisOutputs ); // Store the input sums.
			applyActivationFunctionDerivativeToTheLayer(Derivatives.back(), layer); // Apply the activation function derivation for stored input sums.

      applyActivationFunctionToTheLayer(&thisOutputs, layer); // Turn those sums into outputs.

			Outputs.push_back( thisOutputs ); // Add the neuron outputs for each layer.
		}


// cout<<"\nDERIVATIONS:";
// for(uint i = 0; i<Derivatives.size(); i++){
// 	cout<<"\n  -layer: "<<i+2<<" -->";
// 	printVector(Derivatives.at(i));
// }
// cout<<"\nOUTPUTS:";
// for(uint i = 0; i<Outputs.size(); i++){
// 	cout<<"\n  -layer: "<<i+1<<" -->";
// 	printVector(Outputs.at(i));
// }
// cout<<"\nEXPECT:     -->";
// printVector(outputData_.at(dataIndex));


		// Temp array stores errors from previous layer (the higher one).
		vector<double> last_errors;

		// Calculate the errors for the output layer.
		for (uint outputIndex = 1; outputIndex <= getNoOutputs(); outputIndex++) {

			double target = outputData_[dataIndex][outputIndex - 1];
			double output = Outputs.back()[outputIndex - 1]; // The last (output layer) outputs of NN.

			// This output neurons error.
			double delta = (target - output) * Derivatives.back()[outputIndex-1];

			last_errors.push_back( delta );
		}

		// Add output errors.
		Errors.push_back( last_errors );


		// Backpropagate the error throught the rest of the layers.
		for (uint layer = getNoLayers() - 1; layer > 1; layer--) {

			vector<double> thisErrors; // Stores errors of this layer.

			// Calculate the errors for the this layers neurons.
			for (uint neuronIndex = 1; neuronIndex <= npLayer_[layer-1]; neuronIndex++) {

				double delta = 0;
				// For each neuron from the higher layer.
				for (uint lastN = 1; lastN <= npLayer_[layer]; lastN++) {

          vector<double> wghts;
          getWeights(layer+1, lastN, wghts);

					// Weight pointing to this neuron * error of the last neuron (skipping bias).
					delta += wghts[neuronIndex] * Errors.back().at(lastN - 1);

				}

				// Derivatives are stored from the second layer to higher layers.
				delta *= Derivatives.at(layer-2).at(neuronIndex-1); // Multiply by this neurons derivative.

				thisErrors.push_back( delta );
			}

			Errors.push_back( thisErrors );
		}


// cout<<"\nERRORS:";
// for(uint i = 0; i<Errors.size(); i++){
// // for(uint i = Errors.size()-1; i >= 0; i--){
// 	cout<<"\n  -layer: "<<getNoLayers()-i<<" -->";
// 	printVector(Errors.at(i));
// }}


		// Update the weights.
		for(uint layer = 2; layer <= getNoLayers(); layer++)
		{
			for(uint neuron = 1; neuron <= npLayer_[layer-1]; neuron++)
			{
				vector<double> weights; // Weights for a single neuron in layer.
				getWeights(layer, neuron, weights);


				// Update the bias value.
				weights[0] = weights[0] + learningRate * Errors[getNoLayers()-layer][neuron-1] * 1;

				// Update the weights values.
				for(uint w = 1; w < weights.size(); w++)
				{																				// Error for this neuron (delta)         * Output of the lower layer (or ultimately inputs)
					weights[w] = weights[w] + learningRate * Errors[getNoLayers()-layer][neuron-1] * Outputs[layer-2][w-1];
				}


// cout << "\nOld Weights:";
// printVector(weights);

				setWeights(layer, neuron, weights);

// cout << "\nNew Weights:";
// printVector(weights);

			}
		}
	}
}


void NeuralNetwork::printVector(vector<double>& vect)
{
	for(uint i=0; i<vect.size(); i++)
	{
		cout<<" "<<vect.at(i);
	}
}
