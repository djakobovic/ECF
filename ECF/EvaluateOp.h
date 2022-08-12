#ifndef EvaluateOp_h
#define EvaluateOp_h

#include "Individual.h"

/**
 * \ingroup evol main
 * \brief Evaluation base class
 *
 * The user must inherit and implement a problem specific evaluation operator. 
 * The operator must implement EvaluateOp::evaluate method. 
 * Optional methods are: EvaluateOp::registerParameters (to define user parameters in conf. file) 
 * and EvaluateOp::initialize (to read paramaters and/or perform evaluator initialization).
 */

class EvaluateOp
{
public:
	/**
	 * \brief Evaluate a single individual. Method must create and return a Fitness object.
	 */
	virtual FitnessP evaluate(IndividualP) = 0;

	/**
	 * \brief Register evaluator parameters. Called before EvaluateOp::initialize method.
	 */
	virtual void registerParameters(StateP)	{}

	/**
	 * \brief Initialize the evaluator. Called before first evaluation occurs.
	 */
	virtual bool initialize(StateP)
	{	return true;	}

	virtual ~EvaluateOp()
	{	}
};
typedef boost::shared_ptr<EvaluateOp> EvaluateOpP;

#endif // EvaluateOp_h

