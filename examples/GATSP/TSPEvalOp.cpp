#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include <ECF/ECF.h>
#include "TSPEvalOp.h"


void TSPEvalOp::registerParameters(StateP state)
{
	state->getRegistry()->registerEntry("tsp.infile", (voidP) (new std::string), ECF::STRING);
}


bool TSPEvalOp::initialize(StateP state)
{
	if(!state->getRegistry()->isModified("tsp.infile")) {
		state->getLogger()->log(1, "Error: no input file defined for TSP! (parameter 'tsp.infile'");
		return false;
	}

	voidP sptr = state->getRegistry()->getEntry("tsp.infile"); // get parameter value
	std::string filePath = *((std::string*) sptr.get()); // convert from voidP to user defined type

	std::ifstream iFile(filePath.c_str());
	std::string line;
	if(!iFile.is_open()) {
		state->getLogger()->log(1, "Error: can't open input file " + filePath);
		return false;
	}

	// read dimension (number of cities)
	do {
		getline(iFile,line);
	}while(line.find("DIMENSION",0) == std::string::npos);
	std::stringstream ss(line.substr(line.find(":") + 1));
	ss >> dimension;

	// set dimension for permutation genotype
	state->getRegistry()->modifyEntry("Permutation.size", (voidP) new uint(dimension));

	// reinitialize population with updated size
	state->getPopulation()->initialize(state);

	// parse TSP data type 
	do {
		getline(iFile,line);
	}while(line.find("EDGE_WEIGHT_TYPE",0) == std::string::npos);
	std::stringstream ss_type(line.substr(line.find(":") + 1));
	string dataType;
	ss_type >> dataType;

	if(dataType == "EUC_2D") {

		// read coordinates, calculate euclidian distances 
		do {
			getline(iFile,line);
		}while(line.find("NODE_COORD_SECTION",0) == std::string::npos);

		coordinates.resize(dimension);
		for(int i = 0; i < dimension; i++) {
			coordinates[i].resize(2);
			double datum;
			iFile >> datum;
			iFile >> coordinates[i][0];
			iFile >> coordinates[i][1];
		}

		weights.resize(dimension);
		for(int i = 0; i < dimension; i++) {
			weights[i].resize(dimension);
			for(int j = 0; j < dimension; j++) {
				double diffX = coordinates[i][0] - coordinates[j][0];
				double diffY = coordinates[i][1] - coordinates[j][1];
				weights[i][j] = (int) (0.5 + sqrt(1. * diffX * diffX + diffY * diffY));
			}
		}
		return true;
	
	} else if (dataType == "EXPLICIT") {
		do {
			getline(iFile,line);
		}while(line.find("EDGE_WEIGHT_FORMAT",0) == std::string::npos);
		std::stringstream ss_format(line.substr(line.find(":") + 1));
		string dataFormat;
		ss_format >> dataFormat;
		if(dataFormat == "FULL_MATRIX") {
			// read distances in full matrix form
			do {
				getline(iFile,line);
			}while(line.find("EDGE_WEIGHT_SECTION",0) == std::string::npos);

			weights.resize(dimension);
			for(int i = 0; i < dimension; i++) {
				weights[i].resize(dimension);
				for(int j = 0; j < dimension; j++) {
						iFile >> weights[i][j];
				}
			}
			return true;
		}

	}

	// if all fails:
	// unrecognized TSP data type
	state->getLogger()->log(1, "TSP initializer: can't recognize TSP data in " + filePath
		+ "\n(supported TSP instances: \n\t- type \"EDGE_WEIGHT_TYPE: EUC_2D\" \n\t- type \"EDGE_WEIGHT_TYPE: EXPLICIT\" with "
		+ "\"EDGE_WEIGHT_FORMAT: FULL_MATRIX\")");
	return false;
}


FitnessP TSPEvalOp::evaluate(IndividualP individual)
{
	// minimize travel distance, so use FitnessMin
	FitnessP fitness (new FitnessMin);

	// get Permutation genotype from the individual
	Permutation::Permutation* perm = (Permutation::Permutation*) individual->getGenotype().get();
	// (you can also use boost smart pointers:)
	//PermutationP perm = boost::static_pointer_cast<Permutation::Permutation> (individual->getGenotype());

	int fitnessV = 0;
	// genotype Permutation keeps a vector of indexes named 'variables'
	uint size = (uint) perm->variables.size();
	for(uint i = 0; i < size - 1; i++){
		// the length of each route is the sum of distances (weights) between each city in a route
		fitnessV += weights[perm->variables[i]][perm->variables[i + 1]];
	}
	fitnessV += weights[perm->variables[0]][perm->variables[dimension - 1]];
	
	fitness->setValue(fitnessV);
	return fitness;
}
