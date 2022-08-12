#include <ECF/ECF.h>
#include "FunctionMinEvalOp.h"


int main(int argc, char **argv)
{
	StateP state (new State);

	// set the evaluation operator
	state->setEvalOp(new FunctionMinEvalOp);

	state->initialize(argc, argv);
	state->run();

	return 0;
}
