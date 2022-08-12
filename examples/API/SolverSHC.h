#include "Solver.h"


class SolverSHC : public Solver
{
public:
	SolverSHC();
	bool advanceGeneration(StateP state, DemeP deme);
};
typedef boost::shared_ptr<SolverSHC> SolverSHCP;
