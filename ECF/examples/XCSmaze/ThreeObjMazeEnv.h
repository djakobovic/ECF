#ifndef ThreeObjMazeEnv_h
#define ThreeObjMazeEnv_h

#include "MazeEnv.h"
#undef XCS_STEP_DEBUG
//#undef XCS_DEBUG
#include <string>

class ThreeObjMazeEnv : public MazeEnv {
private:
	double energy;
	double maintenance;
	double lastEnergy;
	double lastMaintenance;

	double reward;
	double moveCost;

public:
	ThreeObjMazeEnv (StateP s, double moveCost);

	GenotypeP getInput();
	bool initialize();
	bool reset();
	bool nextTrial();

	bool isOver();

	FitnessP evaluate (IndividualP ind);
	
};

typedef boost::shared_ptr<ThreeObjMazeEnv> ThreeObjMazeEnvP;

ThreeObjMazeEnv::ThreeObjMazeEnv(StateP s, double moveCost) : MazeEnv(s){
	
	energy = 0;
	reward = 0;
	maintenance = 0;

	this->moveCost = moveCost;
}

#pragma region Environmet methods
GenotypeP ThreeObjMazeEnv::getInput() {

	BitStringP input = static_cast<BitStringP> (new BitString::BitString);

#ifdef XCS_DEBUG
	for (int i = 0; i < nRows; i++) {
		for (int j = 0; j < nColumns; j++) 
			if (i == position.first && j == position.second)
				if (maintenance > 0.5)
					std::cout << "X";
				else
					if (energy > 0.5)
						std::cout << "+";
					else
						std::cout << "-";
			else std::cout << maze[i][j];
			std::cout << std::endl;
	}
	std::cout << "Energy level: " << energy << std::endl;
	std::cout << "Maintenance level: " << maintenance << std::endl;
#endif

	input = boost::dynamic_pointer_cast<BitString::BitString> (MazeEnv::getInput());
	input->bits[24] = (energy > 0.5);
	input->bits[25] = (maintenance > 0.5);

	if (displayStatEvent) displayStatEvent("Getting new input from environment");
	return input;
}

bool ThreeObjMazeEnv::initialize() {
	
	if (!MazeEnv::initialize()) return false;

	reset();

	if (displayStatEvent) displayStatEvent("Three objectives maze environment initialized");
	return true;
}

bool ThreeObjMazeEnv::reset(){
	
	if (!MazeEnv::reset()) return false;

	energy = state->getRandomizer()->getRandomDouble();
	lastEnergy = energy;
	maintenance = state->getRandomizer()->getRandomDouble();
	lastMaintenance = maintenance;
	
	return true;
}

bool ThreeObjMazeEnv::nextTrial() {
	
	std::stringstream ss;
	ss << "Objective found in " << ntries << " tries";
	if (displayStatEvent) displayStatEvent(ss.str().c_str());

	if (isExploitExperiment) {
		std::fstream dat;
		dat.open(resultsPath.c_str(),std::ios_base::app);
		dat << ntries << " " << reward <<  std::endl;
		dat.close();
	}

#ifdef XCSDEBUG
	std::cout << "Objective found in "<< ntries << " tries";
#endif

	if (!MazeEnv::nextTrial()) return false;

	if (isExploitExperiment) {
		position = lastStartPos;
		energy = lastEnergy;
		maintenance = lastMaintenance;
	} else {
		position = getRandomPosition();
		energy = state->getRandomizer()->getRandomDouble();
		maintenance = state->getRandomizer()->getRandomDouble();
		
		lastEnergy = energy;
		lastStartPos = position;
		lastMaintenance = maintenance;
	}

	return true;
}

bool ThreeObjMazeEnv::isOver() {
	char location = getLocation(position);
	return location == MZ_FOOD || location == MZ_ENERGY || location == MZ_MAINTENANCE; 
}

FitnessP ThreeObjMazeEnv::evaluate (IndividualP ind) {

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
		if (energy < 0.5 && maintenance < 0.5)
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

		if (energy > 0.5 && maintenance < 0.5)
			fitness->setValue(1000);
		else
			fitness->setValue(1);

		std::stringstream ss;
		ss << "Food objective found in " << ntries << " tries";
		if (displayStatEvent) displayStatEvent(ss.str().c_str());

#ifdef XCSDEBUG
	std::cout << "Food objective found in "<< ntries << " tries";
#endif
	}
	
	if (getLocation(position) == MZ_MAINTENANCE) {

		if (maintenance > 0.5)
			fitness->setValue(1000);
		else
			fitness->setValue(1);

		std::stringstream ss;
		ss << "Maintenance objective found in " << ntries << " tries";
		if (displayStatEvent) displayStatEvent(ss.str().c_str());

#ifdef XCSDEBUG
	std::cout << "Maintenance objective found in "<< ntries << " tries";
#endif
	}

	std::stringstream ss;
	ss << "Move evaluated with " << fitness->getValue();
	if (displayStatEvent) displayStatEvent(ss.str().c_str());

	reward = fitness->getValue();
	return fitness;
}
#pragma endregion

#endif // ThreeObjMazeEnv_h

