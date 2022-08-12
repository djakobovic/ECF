#ifndef AntEvalOp_h
#define AntEvalOp_h


/**
 * \defgroup ant Artificial Ant Example (GP)
 * \ingroup examples
 * \brief Artificial ant example
 *
 *  \par Description
 *  Evolves a control program for the artificial ant. The objective is to traverse
 *  a given environment and collect as many food items as possible. The ant 'sees' only
 *  a single location in front, and can only turn left, right or move forward. The food
 *  is eaten if the ant moves in the same location.
 *  The environments (food trails) are organized in two files (specified in the config file):
 *  - learning_trails.txt: trails used for learning (ant evolution)
 *  - test_trails.txt: trails used for testing the best evolved ant
 *
 *  \par 
 *  Each trail defines its dimensions, the total number of food items and the maximum allowed
 *  number of moves. You can add or change the existing trails in those files. 
 *
 *  \par
 *  Additional files:
 *  - koza_solution.txt: contains the best solution from Koza 1992 (can be loaded and simulated, see main.cpp)
 *
 *  \par Function set
 *  - IFA: If there is food ahead, execute the first subtree, else execute the second subtree.
 *  - P2: execute the first subtree and then the second subtree.
 *  - P3: execute all three subtrees in sequence.
 *
 *  \par Terminal set
 *  - L: Turn 90 degrees left
 *  - R: Turn 90 degrees right
 *  - M: Move one square ahead
 *
 *  \par Fitness
 *  Number of collected food items, within the maximum number of moves.
 *
 *  \par Reference
 *  John R. Koza, "Genetic Programming I: On the Programming of Computers by Means
 *  of Natural Selection", MIT Press, 1992, pages 147-161.
 */


/**
 * \ingroup ant
 * \brief Artificial ant evaluation class (and environment simulator)
 */
class AntEvalOp : public EvaluateOp
{
	friend ostream& operator<<(ostream& os, AntEvalOp& ant);
protected:
	// ant properties
	uint x_, y_;
	uint facing_;
	uint moves_;
	uint foodEaten_;

	// environments properties
	std::vector<uint> rowNo;
	std::vector<uint> columnNo;
	std::vector<uint> maxSteps;
	std::vector<uint> foodNo;
	std::vector<char*> board;

	// temp enviroments properties
	uint tmpRow;
	uint tmpColumn;
	uint tmpMaxSteps;
	uint tmpFoodNo;
	uint boardNo;
	uint boardNo2;
	char *tmpBoard;

	StateP state_;

	// interactive mode
	static const char LEFT, RIGHT, UP, DOWN;
	string currentTree;
	void showStep(string action);

public:
	// flags
	static bool trace;	//!< trace the ant's movement in the simulator (for visual output)
	static bool step;	//!< show the ant's movement interactive step by step (no effect if trace is false!)

	void registerParameters(StateP);
	FitnessP evaluate(IndividualP individual);
	bool initialize(StateP);

	// function set
	void turnLeft();
	bool facingFood();
	void turnRight();
	void moveAhead();

};
typedef boost::shared_ptr<AntEvalOp> AntEvalOpP;

#endif
