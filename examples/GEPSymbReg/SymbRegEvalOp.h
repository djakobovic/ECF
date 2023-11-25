#ifndef SymbRegEvalOp_h
#define SymbRegEvalOp_h


/**
 * \defgroup symbreg Symbolic regression (GP)
 * \ingroup examples
 * \brief Symbolic regression example - illustration for Tree genotype (see more on this example in help/tutorial.html)
 *
 * \par Description
 * The objective is to find a symbolic form of an unknown function (i.e. find a mapping between the given set 
 * of input and output values). 
 * The fitness is defined with total error for the given set of output values.
 * 
 * \par Tree genotype parameters (in the config file)
 * - set of functions (inner tree nodes)
 * - set of terminals (variables and constants, tree leaves)
 * - min and max tree depth
 *
 */
 
 
/**
 * \ingroup symbreg
 * \brief Symbolic regression evaluation operator.
 */
#include <cmath>
#include <ECF/ECF.h>
#include <fstream>
class SymbRegEvalOp : public EvaluateOp
{
public:
	FitnessP evaluate(IndividualP individual);
	bool initialize(StateP);
	void registerParameters(StateP);
	bool csvRead(StateP state, std::string entry, std::vector<double>* vec);
	std::vector<double> x;
	std::vector<double> y;
	std::vector<double> f;
	uint nSamples;
};
typedef std::shared_ptr<SymbRegEvalOp> SymbRegEvalOpP;

#endif // SymbRegEvalOp_h
