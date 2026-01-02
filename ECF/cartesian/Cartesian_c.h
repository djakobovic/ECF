#ifndef Cartesian_h
#define Cartesian_h
#include "../ECF_base.h"
#include "../Genotype.h"


namespace Cartesian {

class FunctionSet;
typedef std::shared_ptr<FunctionSet> FunctionSetP;

/**
CGP je trenutno vektor indeksa funkcija i oznaka prethodnih gena kao operanada.
TODO: preraditi u vektor gena, gdje jedan gen predstavlja skup indeksa cvora u CGP mrezi.
*/
class Cartesian : public std::vector<uint>, public Genotype
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

	uint getGenomeSize();

	/**
	Build random genotype choosing input connections, outputs and functions
	*/
	void buildRandomGenome();

	/**
	Return result for required inputs (optional: from node with index funcNum) 
	*/
	void evaluate(std::vector<double>& inputData, std::vector<double>& results);

	uint randomConnectionGenerator(uint column);

	FunctionSetP functionSet;		//!< function nodes

public:
	StateP state_;					//!< local copy of state

	// user defined parameters
	uint nVariables;				//!< number of input variables
	uint nConstants;				//!< number of input constants
	uint nOutputs;					//!< number of final outputs
	uint nRows;						//!< number of rows
	uint nCols;						//!< number of columns
	uint nLevelsBack;				//!< levels back parameter		
	uint nFunctions;				//!< number of functions

	// derived parameters
	uint nInputs;					//!< total number of inputs (including constants)
	uint maxArity;					//!< max number of inputs for all function nodes (gates)
};

}
typedef std::shared_ptr<Cartesian::Cartesian> CartesianP;

#endif // Cartesian_h