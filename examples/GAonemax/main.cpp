#include <ECF/ECF.h>
#include <cstdio>
#include "OneMaxEvalOp.h"

int main(int argc, char **argv)
{
	StateP state (new State);

	// set the evaluation operator
	state->setEvalOp(new OneMaxEvalOp);

	state->initialize(argc, argv);
	state->run();

	return 0;
}
