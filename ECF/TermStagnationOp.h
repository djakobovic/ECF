#ifndef TermStagnationOp_H
#define TermStagnationOp_H

/**
 * \ingroup evol term
 * \brief Termination operator: terminates when no improvement occurs in best individual for a given number of generations.
 */
class TermStagnationOp : public Operator
{
public:
	void registerParameters(StateP);
	bool initialize(StateP);
	bool operate(StateP);
protected:
	uint termStagnation_;
};
typedef boost::shared_ptr<TermStagnationOp> TermStagnationOpP;

#endif // TermStagnationOp_H
