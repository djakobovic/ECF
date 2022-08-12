#ifndef SingleObjMazeEnv_h
#define SingleObjMazeEnv_h

#include "MazeEnv.h"
//#undef XCS_STEP_DEBUG
//#undef XCS_DEBUG
#include <cstring>

class SingleObjMazeEnv : public MazeEnv {
private:
	
public:
	SingleObjMazeEnv(StateP s);

	GenotypeP getInput();
	bool initialize();
	bool nextTrial();

	bool isOver();
	FitnessP evaluate (IndividualP ind);
};

typedef boost::shared_ptr<SingleObjMazeEnv> SingleObjMazeEnvP;


SingleObjMazeEnv::SingleObjMazeEnv(StateP s) : MazeEnv(s){

}

#pragma region Environmet methods
GenotypeP SingleObjMazeEnv::getInput() {

#ifdef XCS_DEBUG
	for (int i = 0; i < nRows; i++) {
		for (int j = 0; j < nColumns; j++) 
			if (i == position.first && j == position.second)
				std::cout << "X";
			else std::cout << maze[i][j];
			std::cout << std::endl;
	}
#endif
	return MazeEnv::getInput();
	
}

bool SingleObjMazeEnv::initialize() {
	
	if (!MazeEnv::initialize()) return false;
	reset();

	if (displayStatEvent) displayStatEvent("Single-objective maze environment initialized");
	return true;
}

bool SingleObjMazeEnv::nextTrial() {
	
	std::stringstream ss;
	ss << "Objective found in "<< ntries << " tries";
	if (displayStatEvent) displayStatEvent(ss.str().c_str());

	if (isExploitExperiment) {
		std::fstream dat;
		dat.open(resultsPath.c_str(),std::ios_base::app);
		dat << ntries << std::endl;
		dat.close();
	}

	return MazeEnv::nextTrial();
}

bool SingleObjMazeEnv::isOver() {
	return getLocation(position) == MZ_FOOD; 
}

FitnessP SingleObjMazeEnv::evaluate (IndividualP ind) {

	FitnessP fitness = MazeEnv::evaluate(ind);

	if (getLocation(position) == MZ_FOOD) fitness->setValue(1000);
	else fitness->setValue(0);
	
	std::stringstream ss;
	ss << "Move evaluated with " << fitness->getValue();
	if (displayStatEvent) displayStatEvent(ss.str().c_str());

	return fitness;

}
#pragma endregion 

#endif // SingleObjMazeEnv_h

