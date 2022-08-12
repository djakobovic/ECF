#ifndef AlgGenHookeJeeves_h
#define AlgGenHookeJeeves_h

#include "Algorithm.h"


/**
 * \brief new algorithm, in development
 */
class GenHookeJeeves : public Algorithm
{
public:
	GenHookeJeeves();
	bool advanceGeneration(StateP state, DemeP deme);
	bool initialize(StateP state);
	void registerParameters(StateP state);

protected:
	std::vector<double> delta_;
	std::vector<bool> changed_;
	std::vector<bool> converged_;
	uint convergedTotal_;
	double precision_, initialMove_;
	double lbound_, ubound_;
	bool localOnly_;
	bool areGenotypesAdded_;
	SelectionOperatorP selFitPropOp_, selBestOp_, selRandomOp_;
};
typedef boost::shared_ptr<GenHookeJeeves> GenHookeJeevesP;

#endif // AlgGenHookeJeeves_h

