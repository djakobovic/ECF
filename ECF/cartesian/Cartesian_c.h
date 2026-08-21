#ifndef Cartesian_h
#define Cartesian_h
#include "../ECF_base.h"
#include "../Genotype.h"
#include "Node.h"
#include "Function.h"


namespace Cartesian {

/**
* \defgroup cgp Cartesian
* \ingroup genotypes
*/

class FunctionSet;
typedef std::shared_ptr<FunctionSet> FunctionSetP;


/**
 * \ingroup genotypes cgp
 * CGP (Cartesian genetic programming) is implemented as a vector of Nodes and final output connections.
 * 
 * Nodes can be input nodes (variables and constants) or function nodes.
 * A function Node points to a function primitive which has a predefined number of arguments.
 */
class Cartesian : public Genotype
{
public:
	Cartesian(void);
	~Cartesian(void);

	Cartesian(const Cartesian& other) = default;

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
	std::vector<CrossoverOpP> getCrossoverOp();

	/**
	 * Create and return a vector of mutation operators
	 */
	std::vector<MutationOpP> getMutationOp();

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

	/**
	 * Returns number of mutable genotype elements (function and output nodes)
	 */
	uint getGenomeSize();

	/**
	 * Build random genotype choosing functions and argument connections
	 */
	void buildRandomGenome();

	/**
	 * Calculate CGP results for given inputs 
	 */
	void evaluate(std::vector<double>& inputData, std::vector<double>& results);

	/**
	 * Return a random connection from given column, considering levels_back
	 */
	uint randomNodeInputConnection(uint column);

	/**
	 * Return a random connection for the output (currently excludes input nodes)
	 */
	uint randomOutputConnection();

	/**
	 * Build a vector with indexes of active function nodes (excluding input nodes)
	 */
	void getActiveFunctionNodes(std::vector<uint>&);

protected:
	void addRecursivelyActiveFunctionNodes(std::vector<bool>& activeFlags, uint node);

public:
	StateP state_;					//!< local copy of state
	FunctionSetP functionSet_;		//!< pointer to function set
	std::vector<Node> nodes_;       //!< genotype nodes (input nodes and function nodes)
	std::vector<uint> outputs_;     //!< output nodes
	std::vector<double> constants_; //!< input nodes with constant values

	// user defined parameters
	uint nVariables_;				//!< number of input variables
	uint nConstants_;				//!< number of input constants
	uint nOutputs_;					//!< number of final outputs
	uint nRows_;					//!< number of rows
	uint nCols_;					//!< number of columns
	uint nLevelsBack_;				//!< levels back parameter		
	uint nFunctions_;				//!< number of used functions

	// derived parameters
	uint nInputs_;					//!< total number of inputs (variables and constants)
	uint maxArity_;					//!< max number of inputs for all function nodes
};

}
typedef std::shared_ptr<Cartesian::Cartesian> CartesianP;

#endif // Cartesian_h