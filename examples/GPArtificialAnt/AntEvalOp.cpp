#include <cmath>
#include <ECF/ECF.h>
#include "AntEvalOp.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
using namespace std;


bool AntEvalOp::trace = false;
bool AntEvalOp::step = false;
const char AntEvalOp::LEFT  = '<';
const char AntEvalOp::RIGHT = '>';
const char AntEvalOp::UP    = '^';
const char AntEvalOp::DOWN  = 'v';


/**
 * \brief Output the current state of the board (show ant movements)
 */
ostream& operator<<(ostream& os, AntEvalOp& ant)
{
	for (uint j = 0; j < ant.tmpRow; j++) {
		os << endl;
		for (uint k = 0; k < ant.tmpColumn; k++)
			os << ant.tmpBoard[j * ant.tmpColumn + k];
	}
	os << endl;
	return os;
}


void AntEvalOp::registerParameters(StateP state)
{
	state->getRegistry()->registerEntry("learning_trails", (voidP) (new std::string("learning_trails.txt")), ECF::STRING);
	state->getRegistry()->registerEntry("test_trails", (voidP) (new std::string("test_trails.txt")), ECF::STRING);
}


/**
 * \brief Initialize the simulator, read environments from input file.
 */
bool AntEvalOp::initialize(StateP state)
{
	state_ = state;
	// set the environment pointer to this object (so Tree elements can access the simulator)
	state->getContext()->environment = this;

	voidP sptr = state->getRegistry()->getEntry("learning_trails"); // get parameter value
	std::string filePath = *((std::string*) sptr.get()); // convert from voidP to user defined type

	ifstream file;
	file.open(filePath.c_str());

	if(!file.is_open()) {
		ECF_LOG_ERROR(state, "Error: can't open input file " + filePath);
		return false;
	}

	// read number of training trails
	file >> boardNo;

	// read parameters for every trail
	for (uint k = 0; k < boardNo; k++) {
		tmpRow = tmpColumn = tmpMaxSteps = 0;
		file >> tmpRow;
		file >> tmpColumn;
		file >> tmpFoodNo;
		file.ignore(1);
		file >> tmpMaxSteps;

		rowNo.push_back(tmpRow);
		columnNo.push_back(tmpColumn);
		maxSteps.push_back(tmpMaxSteps);
		foodNo.push_back(tmpFoodNo);

		tmpBoard = (char*) malloc (tmpRow * tmpColumn);

		// read trail
		for (uint i = 0; i < tmpRow; i++ )
			for (uint j = 0; j < tmpColumn; j++)
				file >> tmpBoard[i * tmpColumn + j];

		board.push_back(tmpBoard);
	}

	file.close();

	return true;
}


/**
 * \brief	Evaluation function, simulates ant movement and counts the eaten food.
 */
FitnessP AntEvalOp::evaluate(IndividualP individual)
{
	// greater fitness means better individual
	FitnessP fitness (new FitnessMax);

	// get tree from the individual
	Tree::Tree* tree = (Tree::Tree*) individual->getGenotype().get();

	//create output if necessary (for subsequent evaluation)
	ofstream output;
	if (trace) {
		currentTree = tree->toString();
		output.open("./output.txt");
		output << "Number of trails: " << boardNo << endl;
	}

	int allTheFood = 0;
	for (uint i = 0; i < boardNo; i++) {
		// reset ant properties
		x_ = y_    = 0;
		moves_     = 0;
		facing_    = 0;
		foodEaten_ = 0;

		// read one of the 'boards'
		tmpRow = rowNo[i];
		tmpColumn = columnNo[i];
		tmpMaxSteps = maxSteps[i];
		tmpFoodNo = foodNo[i];

		tmpBoard = (char*) malloc (tmpRow * tmpColumn);

		// create tmpBoard for every individual
		for (uint j = 0; j < tmpRow; j++)
			for (uint k = 0; k < tmpColumn; k++)
				tmpBoard[j * tmpColumn + k] = board[i][j * tmpColumn + k];

		// if the food is on position (0,0)
		if (tmpBoard[0] == 'x')
			foodEaten_++;
		// if trace is set, mark where the ant was
		if (trace)
			if (tmpBoard[0] == 'x')
				tmpBoard[0] = -2;
			else tmpBoard[0] = 'o';

		// repeat moving around until maxSteps is reached or all the food was eaten
		while(moves_ < tmpMaxSteps && foodEaten_< tmpFoodNo) {
			tree->execute(this);
		}
		allTheFood += foodEaten_;

		// if trace is set, save the 'board' in output file
		if (trace) {
			output << "Dimension: " << tmpColumn << "x" << tmpRow << endl;
			output << "Food: " << tmpFoodNo << endl;
			output << "Max Steps: " << tmpMaxSteps << endl;
			output << "Fitness (eaten food): " << foodEaten_ << endl;
			output << *this << endl;
		}

		// and print it on stdout
		if (trace) {
			cout << *this;
			cout << "Eaten food: " << foodEaten_ << endl;
		}
		free(tmpBoard);
	}

	// total fitness - after the ant traverses all the enviroments
	if (trace) {
		cout << "Food eaten in all the enviroments: " << allTheFood << endl;
		output << "Total fitness: " << allTheFood << endl;
		output.close();
	}

	fitness->setValue(allTheFood);

	return fitness;
}


/**
 * \brief	Turn ant on the left.
 */
void AntEvalOp::turnLeft()
{
	if (moves_ >= tmpMaxSteps) return;
	moves_++;
	// four directions: 0, 1, 2, 3
	facing_ = (facing_ - 1) % 4;
	if(trace && step)
		showStep("left");
}


/**
 * \brief	Turn ant on the right.
 */
void AntEvalOp::turnRight()
{
	if (moves_ >= tmpMaxSteps) return;
	moves_++;
	// four directions: 0, 1, 2, 3
	facing_ = (facing_ + 1) % 4;
	if(trace && step)
		showStep("right");
}


/**
 * \brief	Move ant ahead.
 */
void AntEvalOp::moveAhead()
{
	if (moves_ >= tmpMaxSteps) return;
	moves_++;
	// move forward
	// facing: 0,2 - right left
	//		   1,3 - down up
	switch (facing_)
	{
		case 3: //up
			y_ = (y_ - 1) % tmpRow;
			break; 
		case 2: //left
			x_ = (x_ - 1) % tmpColumn;
			break; 
		case 1: //down
			y_ = (y_ + 1) % tmpRow; 
			break; 
		case 0: //right
			x_ = (x_ + 1) % tmpColumn;
			break; 
		default: 
			break;
	}

	// if ant is on the food, remove food from 'board' and increase the number of eaten food
	if (tmpBoard[y_*tmpColumn+x_] == 'x') {
		foodEaten_++;
		if (trace) 
			tmpBoard[y_*tmpColumn+x_] = -2;
		else 
			tmpBoard[y_*tmpColumn+x_] = '.';
	}
	if(trace) {
		if(tmpBoard[y_*tmpColumn+x_] == '.')
			tmpBoard[y_*tmpColumn+x_] = 'o';
		if(step)
			showStep("move");
	}
}


/**
 * \brief	Check if there's food in front.
 */
bool AntEvalOp::facingFood()
{
	int x1,y1;
	x1 = x_;
	y1 = y_;
	switch (facing_)
	{
		case 3: y1 = (y_==0 ? tmpRow-1 : y_-1); break;    // up
		case 2: x1 = (x_==0 ? tmpColumn-1 : x_-1); break; // left
		case 1: y1 = (y_+1)%tmpRow; break;                // down
		case 0: x1 = (x_+1)%tmpColumn; break;             // right
	}
	if (tmpBoard[y1 * tmpColumn + x1] == 'x')
		return true;
	return false;
}


/**
 * \brief	Show the ant's current action (interactive)
 */
void AntEvalOp::showStep(string action)
{
	cout << "Ant: " << currentTree << endl;
	cout << "Action: " << action << ", moves: " << moves_ << "/" << tmpMaxSteps << endl;

	char ant, position = tmpBoard[y_ * tmpColumn + x_];
	switch (facing_) {
		case 3: ant = UP; break;
		case 2: ant = LEFT; break;
		case 1: ant = DOWN; break;
		case 0: ant = RIGHT; break;
	}

	tmpBoard[y_ * tmpColumn + x_] = ant;
	cout << *this;
	tmpBoard[y_ * tmpColumn + x_] = position;

	cout << "(Enter to continue...)";
	string dummy;
	getline(cin, dummy);
}