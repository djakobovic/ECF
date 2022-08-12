#ifndef MazeEnv_h
#define MazeEnv_h

#include "../../ECF.h"

using namespace std;

#define MAX_ROWS 1000
#define MAX_COLUMNS 1000

//Maze elements definitions
const char MZ_EMPTY = '.';
const char MZ_FOOD = 'F';
const char MZ_WALL = '#';
const char MZ_AGENT = '*';

const char MZ_ENERGY = 'E';
const char MZ_KEY = 'K';
const char MZ_MAINTENANCE = 'M';

class MazeEnv : public Environment {
protected:
	char maze[MAX_ROWS][MAX_COLUMNS];
	std::string mazePath;
	std::string resultsPath;

	int nRows;
	int nColumns;
	std::pair<int, int> position;
	std::pair<int, int> lastStartPos;
	bool isExploitExperiment;
	int generation;

	int ntries;

	bool loadMaze(const char* path);
	std::pair<int, int> getRandomPosition();
	char getLocation(std::pair<int, int> pos);
	
	std::pair<int, int> makeMove(std::pair<int, int> pos, int direction);
	void getLocationBits(char loc, bool* bits);

public:

	MazeEnv(StateP state);

	FitnessP evaluate(IndividualP ind);
	virtual bool isExploit();
	virtual bool nextTrial();
	virtual bool reset();
	virtual bool initialize();
	virtual GenotypeP getInput();


	void (__stdcall *displayStatEvent)(const char *status);
	void (__stdcall *changePositionEvent) (std::pair<int, int> pos);

	char *loadMazeFromFile(const char* path, int *nRows, int *nColumns);
	void setMazeFile(const char* mazeFile);
	void setResultsFile(const char* resultsFile);
};

typedef boost::shared_ptr<MazeEnv> MazeEnvP;
#endif // MazeEnv_h

