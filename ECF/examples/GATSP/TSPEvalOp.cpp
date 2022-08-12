#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include "../../ECF.h"
#include "TSPEvalOp.h"

void TSPEvalOp::registerParameters(StateP state)
{
	state->getRegistry()->registerEntry("tsp.infile", (voidP) (new std::string), ECF::STRING);
}


bool TSPEvalOp::initialize(StateP state)
{
	if(!state->getRegistry()->isModified("tsp.infile")) {
		ECF_LOG_ERROR(state, "Error: no input file defined for TSP! (parameter 'tsp.infile'");
		return false;
	}

	voidP sptr = state->getRegistry()->getEntry("tsp.infile"); // get parameter value
	std::string filePath = *((std::string*) sptr.get()); // convert from voidP to user defined type

	std::ifstream iFile(filePath.c_str());
	std::string line;
	if(!iFile.is_open()) {
		ECF_LOG_ERROR(state, "Error: can't open input file " + filePath);
		return false;
	}

	do {
		getline(iFile,line);
	}while(line.find("DIMENSION",0) == std::string::npos);

	std::stringstream ss(line.substr(line.find(":") + 1));
	ss >> dimension;

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


FitnessP TSPEvalOp::evaluate(IndividualP individual)
	{
		FitnessP fitness = static_cast<FitnessP> (new FitnessMin);

		PermutationP perm = boost::dynamic_pointer_cast<Permutation::Permutation> (individual->getGenotype());

		int fitnessV = 0;

		for(uint i = 0; i < perm->variables.size() - 1; i++){
			fitnessV += weights[perm->variables[i]][perm->variables[i + 1]];
		}
		fitnessV += weights[perm->variables[0]][perm->variables[dimension - 1]];
		

		fitness->setValue(fitnessV);
		return fitness;
	}

