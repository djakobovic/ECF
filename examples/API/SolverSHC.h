#include "Solver.h"


class SolverSHC : public Solver
{
public:
	SolverSHC();
	bool advanceGeneration(StateP state, DemeP deme);
};
typedef std::shared_ptr<SolverSHC> SolverSHCP;
