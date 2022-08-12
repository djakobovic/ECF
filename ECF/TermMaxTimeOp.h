#ifndef TermMaxTimeOp_H
#define TermMaxTimeOp_H

/**
 * \ingroup evol term
 * \brief Termination operator: terminates on a given elapsed time
 */
class TermMaxTimeOp : public Operator
{
public:
	void registerParameters(StateP);
	bool initialize(StateP);
	bool operate(StateP);
protected:
	uint maxTime_;
};
typedef boost::shared_ptr<TermMaxTimeOp> TermMaxTimeOpP;

#endif // TermMaxTimeOp_H
