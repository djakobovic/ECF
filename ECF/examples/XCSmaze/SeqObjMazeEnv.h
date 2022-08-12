#ifndef SeqObjMazeEnv_h
#define SeqObjMazeEnv_h

#include "MazeEnv.h"
#undef XCS_STEP_DEBUG
#undef XCS_DEBUG
#include <string>

class SeqObjMazeEnv : public MazeEnv {
private:
	bool keyVisited;

public:
	SeqObjMazeEnv(StateP s);

	GenotypeP getInput();
	bool initialize();
	bool reset();
	bool nextTrial();

	bool isOver();

	FitnessP evaluate (IndividualP ind);
	
};

typedef boost::shared_ptr<SeqObjMazeEnv> SeqObjMazeEnvP;

SeqObjMazeEnv::SeqObjMazeEnv(StateP s) : MazeEnv(s){
	
	keyVisited = false;
}

#pragma region Environmet methods
GenotypeP SeqObjMazeEnv::getInput() {

	BitStringP input = static_cast<BitStringP> (new BitString::BitString);

#ifdef XCS_DEBUG
	for (int i = 0; i < nRows; i++) {
		for (int j = 0; j < nColumns; j++) 
			if (i == position.first && j == position.second)
				if (keyVisited)
					std::cout << "X";
				else
					std::cout << "?";
			else std::cout << maze[i][j];
			std::cout << std::endl;
	}
#endif

	input = boost::dynamic_pointer_cast<BitString::BitString> (MazeEnv::getInput());
	input->bits[24] = keyVisited;

	if (displayStatEvent) displayStatEvent("Getting new input from environment");
	return input;
}

bool SeqObjMazeEnv::initialize() {
	
	if (!MazeEnv::initialize()) return false;

	reset();

	if (displayStatEvent) displayStatEvent("Sequential multi-objective maze environment initialized");
	return true;
}

bool SeqObjMazeEnv::reset(){
	
	if (!MazeEnv::reset()) return false;
	keyVisited = false;

	return true;
}

bool SeqObjMazeEnv::nextTrial() {
	
	std::stringstream ss;
	ss << "Second objective found in " << ntries << " tries";
	if (displayStatEvent) displayStatEvent(ss.str().c_str());

	if (isExploitExperiment) {
		std::fstream dat;
		dat.open(resultsPath.c_str(),std::ios_base::app);
		dat << ntries << std::endl;
		dat.close();
	}

#ifdef XCSDEBUG
	std::cout << "Second objective found in "<< ntries << " tries";
#endif

	if (!MazeEnv::nextTrial()) return false;
	keyVisited = false;

	return true;	
}

bool SeqObjMazeEnv::isOver() {
	return getLocation(position) == MZ_FOOD && keyVisited; 
}

FitnessP SeqObjMazeEnv::evaluate (IndividualP ind) {

	FitnessP fitness = MazeEnv::evaluate(ind);

	if (getLocation(position) == MZ_KEY) { 
		keyVisited = true;

		std::stringstream ss;
		ss << "First objective found in "<< ntries << " tries";
		if (displayStatEvent) displayStatEvent(ss.str().c_str());

#ifdef XCSDEBUG
	std::cout << "First objective found in "<< ntries << " tries";
#endif
	}

	if (getLocation(position) == MZ_FOOD && keyVisited) {

		//if (generation > 4)
		fitness->setValue(1000);
	}
	
	std::stringstream ss;
	ss << "Move evaluated with " << fitness->getValue();
	if (displayStatEvent) displayStatEvent(ss.str().c_str());

	return fitness;

}
#pragma endregion

#endif // SeqObjMazeEnv_h

