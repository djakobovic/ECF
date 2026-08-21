#ifndef OneMaxEvalOp_h
#define OneMaxEvalOp_h


/**
 * \defgroup onemax OneMax problem
 * \ingroup examples
 * \brief OneMax example - illustration for BitString genotype (see more on this example in https://ecf.readthedocs.io/tutorial.html)
 *
 * \par Description
 * The onemax optimization is a simple search which counts the ones in a bit string.
 * The optimal solution is a string consisting entirely of ones.
 * 
 * \par Parameters (in the config file)
 * - for BitString genotype, only its size must be defined
 *
 */


/**
 * \ingroup onemax
 * \brief OneMax problem evaluation class.
 */
class OneMaxEvalOp : public EvaluateOp
{
public:
	FitnessP evaluate(IndividualP individual);
};
typedef std::shared_ptr<OneMaxEvalOp> OneMaxEvalOpP;



class MyEvalOp : public EvaluateOp
{
public:
	StateP state_;

	bool initialize(StateP state)
	{
		state_ = state;
		return true;
	}
	FitnessP evaluate(IndividualP individual)
	{
		DemeP myDeme = state_->getPopulation()->getLocalDeme();
		for (IndividualP ind : *myDeme) {
			std::cout << ind->index << ": " << ind->toString() << std::endl;
		}
		return (FitnessP)(new FitnessMin);
	}
};


#endif // OneMaxEvalOp_h

