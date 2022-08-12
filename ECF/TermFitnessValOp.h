#ifndef TermFitnessValOp_H
#define TermFitnessValOp_H

/**
 * \ingroup evol term
 * \brief Termination operator: terminates on a given fitness value.
 */
class TermFitnessValOp : public Operator
{
public:
	void registerParameters(StateP);
	bool initialize(StateP);
	bool operate(StateP);

protected:
	double termValue_;
};
typedef boost::shared_ptr<TermFitnessValOp> TermFitnessValOpP;

#endif // TermFitnessValOp_H
