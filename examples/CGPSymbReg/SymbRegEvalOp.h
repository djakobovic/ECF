#ifndef SymbRegEvalOp_h
#define SymbRegEvalOp_h


/**
 * \defgroup cgpsymbreg Symbolic regression with Cartesian Genetic Programming (CGP)
 * \ingroup examples
 * \brief CGP symbolic regression example - illustration for Cartesian genotype 
 *
 * \par Description
 * The objective is to find a symbolic form of an unknown function (i.e. find a mapping between the given set 
 * of input and output values). 
 * The fitness is defined with total error for the given set of output values.
 * 
 * \par Cartesian genotype parameters (in the config file)
 * - set of functions 
 * - set of terminals (input variables and constants)
 * - genotype dimensions (rows, columns)
 *
 */
 
 
/**
 * \ingroup cgpsymbreg
 * \brief CGP symbolic regression evaluation operator.
 */
class CGPSymbRegEvalOp : public EvaluateOp
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

	std::vector< std::vector<double> > inputs_;
	uint nSamples_;
};
typedef std::shared_ptr<CGPSymbRegEvalOp> CGPSymbRegEvalOpP;

#endif // SymbRegEvalOp_h
