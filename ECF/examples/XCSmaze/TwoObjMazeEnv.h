#ifndef TwoObjMazeEnv_h
#define TwoObjMazeEnv_h

#include "MazeEnv.h"

//#undef XCS_STEP_DEBUG
//#undef XCS_DEBUG
#include <string>

class TwoObjMazeEnv : public MazeEnv {
private:
	double energy;
	double lastEnergy;
	double reward;
	double moveCost;

public:
	TwoObjMazeEnv (StateP s, double moveCost);

	GenotypeP getInput();
	bool initialize();
	bool reset();
	bool nextTrial();

	bool isOver();

	FitnessP evaluate (IndividualP ind);
	
};

typedef boost::shared_ptr<TwoObjMazeEnv> TwoObjMazeEnvP;

TwoObjMazeEnv::TwoObjMazeEnv(StateP s, double moveCost) : MazeEnv(s){
	
	ntries = 0;
	energy = 0;
	reward = 0;

	this->moveCost = moveCost;

}

#pragma region Environmet methods
GenotypeP TwoObjMazeEnv::getInput() {

	BitStringP input = static_cast<BitStringP> (new BitString::BitString);

#ifdef XCS_DEBUG
	for (int i = 0; i < nRows; i++) {
		for (int j = 0; j < nColumns; j++) 
			if (i == position.first && j == position.second)
				if (energy > 0.5)
					std::cout << "X";
				else
					std::cout << "?";
			else std::cout << maze[i][j];
			std::cout << std::endl;
	}
	std::cout << "Energy level: " << energy << std::endl;
#endif

	input = boost::dynamic_pointer_cast<BitString::BitString> (MazeEnv::getInput());
	input->bits[24] = (energy > 0.5);

	if (displayStatEvent) displayStatEvent("Getting new input from environment");
	return input;
}

bool TwoObjMazeEnv::initialize() {
	
	if (!MazeEnv::initialize()) return false;

	reset();

	if (displayStatEvent) displayStatEvent("Two objectives maze environment initialized");

	return true;
}

bool TwoObjMazeEnv::reset(){
	
	if (!MazeEnv::reset()) return false;
	energy = state->getRandomizer()->getRandomDouble();
	lastEnergy = energy;

	return true;
}

bool TwoObjMazeEnv::nextTrial() {
	
	if (isExploitExperiment) {
		std::fstream dat;
		dat.open(resultsPath.c_str(),std::ios_base::app);
		dat << ntries << " " << reward <<  std::endl;
		dat.close();
	}

#ifdef XCSDEBUG
	std::cout << "Second objective found in "<< ntries << " tries";
#endif
	
	if (!MazeEnv::nextTrial()) return false;

	if (isExploitExperiment) {
		position = lastStartPos;
		energy = lastEnergy;
	} else {
		position = getRandomPosition();
		energy = state->getRandomizer()->getRandomDouble();
		
		lastEnergy = energy;
		lastStartPos = position;
	}

	return true;
}

bool TwoObjMazeEnv::isOver() {
	char location = getLocation(position);
	return location == MZ_FOOD || location == MZ_ENERGY; 
}

FitnessP TwoObjMazeEnv::evaluate (IndividualP ind) {

	FitnessP fitness = static_cast<FitnessP> (new FitnessMax);
	fitness->setValue(0);

	int move = 0;
	BitStringP bstring = boost::dynamic_pointer_cast<BitString::BitString> (ind->getGenotype(1));

	for (int i =0; i < 3; i++){
		move *= 2;
		if (bstring->bits[i]) move += 1;
	}
	
	//make move only if there is no wall on target location
	pair<int, int> pos = makeMove(position, move);
	if (getLocation(pos) != MZ_WALL) {
		position = pos;
		energy = abs(energy - moveCost); 
		if (changePositionEvent) changePositionEvent(pos);
	}

	if (getLocation(position) == MZ_ENERGY) { 
		if (energy < 0.5)
			fitness->setValue(1000);
		else
			fitness->setValue(1);

		std::stringstream ss;
		ss << "Energy objective found in "<< ntries << " tries";
		if (displayStatEvent) displayStatEvent(ss.str().c_str());

#ifdef XCSDEBUG
	std::cout << "Energy objective found in "<< ntries << " tries";
#endif
	}
	if (getLocation(position) == MZ_FOOD) {

		if (energy > 0.5)
			fitness->setValue(1000);
		else
			fitness->setValue(1);

		std::stringstream ss;
		ss << "Food objective found in " << ntries << " tries";
		if (displayStatEvent) displayStatEvent(ss.str().c_str());
	}
	
	std::stringstream ss;
	ss << "Move " << move << " evaluated with " << fitness->getValue();
	if (displayStatEvent) displayStatEvent(ss.str().c_str());

	reward = fitness->getValue();
	return fitness;

}
#pragma endregion

#endif // TwoObjMazeEnv_h

