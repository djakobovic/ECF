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
class SymbRegEvalOp : public EvaluateOp
{
public:
	FitnessP evaluate(IndividualP individual);
	bool initialize(StateP);
	
	//domain
	std::vector<double> x1;
	std::vector<double> x2;
	std::vector<double> x3;

	//codomain
	std::vector<double> y1;
	std::vector<double> y2;
	std::vector<double> y3;

	std::vector< std::vector<double> > inputs;

	uint nSamples;
};
typedef std::shared_ptr<SymbRegEvalOp> SymbRegEvalOpP;

#endif // SymbRegEvalOp_h
