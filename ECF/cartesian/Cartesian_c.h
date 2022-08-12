#ifndef Cartesian_h
#define Cartesian_h
#include "../ECF_base.h"
#include "../Genotype.h"

typedef unsigned int uint;
using namespace std;

namespace cart {

class FunctionSet;
typedef boost::shared_ptr<FunctionSet> FunctionSetP;

class Cartesian : public vector<uint>, public Genotype
{
public:
	Cartesian(void);
	~Cartesian(void);
	/**
	 * Initialize a genotype object (read parameters, perform sanity check, build data)
	 */
	bool initialize(StateP state);

	/**
	 * Create an identical copy of the genotype object
	 */
	Cartesian* copy();

	/**
	 * Create and return a vector of crossover operators
	 */
	vector<CrossoverOpP> getCrossoverOp();

	/**
	 * Create and return a vector of mutation operators
	 */
	vector<MutationOpP> getMutationOp();

	/**
	 * Register genotype's parameters (called before Genotype::initialize)
	 */
	void registerParameters(StateP state);

	/**
	 * Read genotype data from XMLNode 
	 */
	void read(XMLNode &xCart);

	/**
	 * Write genotype data to XMLNode 
	 */
	void write(XMLNode &xCart);

	uint getGenomeSize();

	/**
	Make random genotype by using rules of choosing input connections, outputs and functions in order to
	make valid genotype.
	*/
	void makeGenotype();
	/**
	Return random unsigned integer for input connection by refering to rules for creating valid genotype
	and current column (in which current node is placed).
	*/
	uint randInputConn(uint currCol);
	/**
	Return random unsigned integer for final output by refering to rules for creating valid genotype.
	*/
	uint randOutput();
	/**
	Return random unsigned integer defining function by refering to rules for creating valid genotype.
	*/
	uint randFunction();
	/**
	Return result for required inputs from node with function with index funcNum.
	*/
	void evaluate(voidP inputs, void* result, uint funcNum);
	/**
	Print genotype on standard output.
	*/
	void printGenotype();
	/**
	Return number of primary inputs.
	*/
	uint getNumOfInputs();
	/**
	Return number of final outputs.
	*/
	uint getNumOfOutputs();
	/**
	Return number of input connections - it defines how many inputs will every node in genotype have.
	*/
	uint getNumOfInputConn();
	/**
	Return all possible constant names.
	*/
	voidP getConstantNames();
	/**
	Return number of rows in genotype.
	*/
	uint getNumOfRows();
	/**
	Return number of columns in genotype.
	*/
	uint getNumOfCols();
	/**
	Return levels back parameter - it defines how many previous columns of cells may have their outputs
	connected to a node in current column (primary inputs are treated in the same way as node outputs).
	*/
	uint getLevelsBack();
	/**
	Return number of variables - inputs to be replaced by elements from domain in evaluation process.
	*/
	uint getNumVars();

	FunctionSetP funcSet;		//!< function nodes

protected:
	StateP state_;					//!< local copy of state

	uint inputs;					//!< number of primary inputs
	uint outputs;					//!< number of final outputs
	uint inputConns;				//!< number of input connections
	uint rows;						//!< number of rows
	uint cols;						//!< number of columns
	uint levelsBack;				//!< levels back parameter		
	uint numVars;				//!< number of variables
	uint numFunc;				//!< number of functions
	voidP constantset;	//!< all possible constants
	
	
};
typedef boost::shared_ptr<Cartesian> CartesianP;
}

#endif // Cartesian_h