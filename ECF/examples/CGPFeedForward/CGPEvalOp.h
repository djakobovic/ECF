#ifndef FUNCTIONMINEVALOP_H_
#define FUNCTIONMINEVALOP_H_

#include <cmath>


/**
 * \defgroup fmin Function minimization
 * \ingroup examples
 * \brief Function minimization example - using FloatingPoint or Binary genotype (see more on this example in help/tutorial.html)
 *
 * \par Description
 * Tries to find the (global) minimum of a continuous multidimensional function.
 * The example uses either FloatingPoint or Binary genotype.
 * 
 * \par Parameters (in the config file)
 * - "function": defines the minimization function to be used (default: 1)
 * - with FloatingPoint genotype, a number of dimensions and the interval [lbound, ubound] must be set
 * - with Binary genotype, a number of dimensions, the interval [lbound, ubound] and precision 
 * (number of digits after the decimal point) must be set
 *
 * \par References
 * - http://www.geatbx.com/docu/fcnindex-01.html 
 * - http://www.gamsworld.org/performance/selconglobal/
 * - http://coco.lri.fr/downloads/download11.06/bbobdocfunctions.pdf 
 */


/**
 * \ingroup fmin
 * \brief Function minimization evaluation class.
 */
class FunctionMinEvalOp : public EvaluateOp 
{
protected:
	uint iFunction_;	//!< function index
public:
	FitnessP evaluate(IndividualP individual);
	void registerParameters(StateP);
	bool initialize(StateP);

	bool showTruth;
};
typedef boost::shared_ptr<FunctionMinEvalOp> FunctionMinEvalOpP;

#endif /* FUNCTIONMINEVALOP_H_ */
