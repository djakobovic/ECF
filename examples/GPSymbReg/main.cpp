#include <ECF/ECF.h>
#include "SymbRegEvalOp.h"

int main(int argc, char **argv)
{
	StateP state (new State);

	// set the evaluation operator
	state->setEvalOp(new SymbRegEvalOp);

	state->initialize(argc, argv);
	state->run();

	return 0;
}
