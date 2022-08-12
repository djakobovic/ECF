#ifndef Solver_Evolution_Strategy_h
#define Solver_Evolution_Strategy_h

#include "Solver.h"


class SolverEvolutionStrategy : public Solver
{
public:
	SolverEvolutionStrategy();
	bool advanceGeneration(StateP state, DemeP deme);
	bool initialize(StateP state);
	void registerParameters(StateP state);

protected:
	uint lambda_;         //!< number of offspring
	uint rho_;            //!< number of parents (1 or 2)
	uint mu_;             //!< the size of the parent population
	uint subPopulations_; //!< how many parent populations are in a deme
	bool plusSelection_;  //!< type of selection (plus or comma)
	SelectionOperatorP  selBestOp_, selRandomOp_;

	static bool compare(IndividualP first, IndividualP second)
	{	return first->fitness->isBetterThan(second->fitness);	}

};
typedef boost::shared_ptr<SolverEvolutionStrategy> SolverEvolutionStrategyP;

#endif // Evolution_Strategy_h

