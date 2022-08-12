#ifndef TermMaxEvalOp_H
#define TermMaxEvalOp_h

/**
 * \ingroup evol term
 * \brief Termination operator: terminates on a given number of fitness evaluations.
 */
class TermMaxEvalOp : public Operator
{
public:
	void registerParameters(StateP);
	bool initialize(StateP);
	bool operate(StateP);
protected:
	uint nEvaluations_;
};
typedef boost::shared_ptr<TermMaxEvalOp> TermMaxEvalOpP;

#endif // TermMaxEvalOp
