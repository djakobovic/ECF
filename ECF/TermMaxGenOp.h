#ifndef TermMaxGenOp_H
#define TermMaxGenOp_H

/**
 * \ingroup evol term
 * \brief Termination operator: terminates on a given number of generations.
 */
class TermMaxGenOp : public Operator
{
public:
	void registerParameters(StateP);
	bool initialize(StateP);
	bool operate(StateP);
protected:
	uint nGenerations_;
};
typedef boost::shared_ptr<TermMaxGenOp> TermMaxGenOpP;

#endif // TermMaxGenOp_H
