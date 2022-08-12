#include "Solver.h"


class SolverTournamentEA : public Solver
{
public:
	SolverTournamentEA();
	bool advanceGeneration(StateP state, DemeP deme);
	bool initialize(StateP state);
	void registerParameters(StateP state);

protected:
	uint nTournament_;	//!< tournament size
	SelectionOperatorP selRandomOp, selWorstOp;

};
typedef boost::shared_ptr<SolverTournamentEA> SolverTournamentEAP;
