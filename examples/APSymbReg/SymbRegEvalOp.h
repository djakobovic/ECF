#ifndef SymbRegEvalOp_h
#define SymbRegEvalOp_h


/**
 * \defgroup apsymbreg Symbolic regression (Analytic Programming)
 * \ingroup examples
 * \brief Symbolic regression example - illustration for APGenotype genotype
 *
 * \par Description
 * The objective is to find a symbolic form of an unknown function (i.e. find a mapping between the given set 
 * of input and output values). 
 * The fitness is defined with total error for the given set of output values.
 * 
 * \par genotype parameters (in the config file)
 * - set of functions (inner tree nodes)
 * - set of terminals (variables and constants, tree leaves)
 * - min and max tree depth
 * - lbound and ubound for floating point representation in AP genotype
 *
 */
 
 
/**
 * \ingroup apsymbreg
 * \brief Symbolic regression evaluation operator (using AP genotype).
 */
class SymbRegEvalOp : public EvaluateOp
{
public:
	FitnessP evaluate(IndividualP individual);
	bool initialize(StateP);
	std::vector<double> domain;
	std::vector<double> codomain;
	uint nSamples;
	bool useAPGenotype;
};
typedef std::shared_ptr<SymbRegEvalOp> SymbRegEvalOpP;

#endif // SymbRegEvalOp_h
