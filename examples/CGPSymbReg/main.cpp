#include <ECF/ECF.h>
#include "SymbRegEvalOp.h"


int main(int argc, char **argv) 
{
	StateP state(new State);

	CartesianP cgp (new Cartesian::Cartesian);
	state->addGenotype(cgp);

	state->setEvalOp(new SymbRegEvalOp);

	state->initialize(argc, argv);
	state->run();

	return 0;
}