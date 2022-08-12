#include "MazeEnv.h"
#include <string>
#include <fstream>
#include <cstring>

MazeEnv::MazeEnv(StateP s) {
	nRows = 0;
	nColumns = 0;
	position = make_pair(0,0);
	state = s;
	ntries = 0;

	displayStatEvent = NULL;
	changePositionEvent = NULL;
	isExploitExperiment = false;
}

GenotypeP MazeEnv::getInput(){
	ntries++;
	BitStringP input = static_cast<BitStringP> (new BitString::BitString);
	input->setGenotypeId(0);	// moramo postaviti indeks genotipa (prvi genotip)
	
	input->initialize(state);	// dobijemo slucajni niz bitova

	//Creating input bits from current animat view in maze
	bool bits[3];
	for (int i = 0; i < 8; i++){
		pair<int, int> pos = makeMove(position, i);
		getLocationBits(getLocation(pos), bits);
		for (int j = 0; j < 3; j++)
			input->bits[i*3+j] = bits[j];
	}
	if (displayStatEvent) displayStatEvent("Getting new input from environment");
	return input;
}

bool MazeEnv::initialize(){
	//loading maze from file
	try {
		loadMaze(mazePath.c_str());
	} catch (std::string text) {
		throw (text);
	}
	return true;
}

bool MazeEnv::reset(){
	position = getRandomPosition();
	lastStartPos = position;

	ntries = 0;
	isExploitExperiment = false;
	return true;
}

bool MazeEnv::nextTrial() {

	isExploitExperiment = !isExploitExperiment;
	ntries = 0;

	if (isExploitExperiment) {
		position = lastStartPos;
	} else {
		position = getRandomPosition();
		lastStartPos = position;
	}
	return true;
}

//Loads maze from file located in 'path'
bool MazeEnv::loadMaze(const char* path){
	
	ifstream mazeFile(path);
	if (!mazeFile) throw (std::string("Can not open maze file: ")+ std::string(path));

	int i = 0;
	while (mazeFile >> maze[i]){
		if (nColumns == 0) nColumns = (int) strlen (maze[i]);
		else if (nColumns != strlen(maze[i]))
			throw (std::string("Wrong length of maze row"));
		i++;
	}
	nRows = i;
	
	std::stringstream ss;
	ss << "Maze description loaded from file: " << path;
	if (displayStatEvent) displayStatEvent(ss.str().c_str());

	mazeFile.close();

	return true;
}

FitnessP MazeEnv::evaluate(IndividualP ind) {
	
	FitnessP fitness = static_cast<FitnessP> (new FitnessMax);
	fitness->setValue(0);
	int move = 0;

	BitStringP bstring = boost::dynamic_pointer_cast<BitString::BitString> (ind->getGenotype(1));
	//TODO napravit neku metodu za pretvaranje bitstringa u int i obratno

	for (int i =0; i < 3; i++){
		move *= 2;
		if (bstring->bits[i]) move += 1;
	}
	
	//make move only if there is no wall on target location
	pair<int, int> pos = makeMove(position, move);
	if (getLocation(pos) != MZ_WALL) {
		position = pos;
		if (changePositionEvent) changePositionEvent(pos);
	}

	return fitness;
}

std::pair<int, int> MazeEnv::getRandomPosition() {
	
	std::pair<int, int> pos;
	do {
		pos.first = state->getRandomizer()->getRandomInteger(nRows);
		pos.second = state->getRandomizer()->getRandomInteger(nColumns);
	}while(getLocation (pos) != MZ_EMPTY );

	return pos;
}


char MazeEnv::getLocation(pair<int, int> pos){
	return maze[pos.first][pos.second];
}

void MazeEnv::getLocationBits(char loc, bool* bits){
	bits[0] = false; bits[1] = false; bits[2] = false;
	switch (loc){
		case MZ_EMPTY: break;
		case MZ_WALL: bits[2] = true; break;
		case MZ_FOOD: bits[1] = true; break;
		case MZ_KEY: bits[0] = true; break;
		case MZ_ENERGY: bits[0] = true; bits[1] = true; break;
	}
}

/* Possible moves enumeration:
	0 1 2
	3 * 4
	5 6 7
	Maze has toroidal shape.
*/
std::pair<int, int> MazeEnv::makeMove(std::pair<int, int> pos, int direction){

//TODO: prebacit da broji od nule
	switch (direction) {
		case (0): --pos.first; --pos.second; break;
		case (1): --pos.first; break;
		case (2): --pos.first; ++pos.second;break;
		case (3): --pos.second; break;
		case (4): ++pos.second; break;
		case (5): ++pos.first; --pos.second; break;
		case (6): ++pos.first; break;
		case (7): ++pos.first; ++pos.second; break;
	}
	if (pos.first < 0) pos.first = nRows -1;
	if (pos.second < 0) pos.second = nColumns -1;
	if (pos.first >= nRows) pos.first = 0;
	if (pos.second >= nColumns) pos.second = 0;

	return pos;
}

//Load maze definition from file located in 'path'
char *MazeEnv::loadMazeFromFile(const char* path, int *nRows, int *nColumns){

	loadMaze(path);
	*nRows = this->nRows;
	*nColumns = this->nColumns;

	return &maze[0][0];
}

void MazeEnv::setMazeFile(const char* mazeFile) {
	mazePath = std::string(mazeFile);
}

void MazeEnv::setResultsFile(const char* resultsFile){
	resultsPath = std::string(resultsFile);
}

bool MazeEnv::isExploit() {
	return isExploitExperiment;
}