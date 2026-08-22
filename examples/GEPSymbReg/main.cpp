#include <ECF/ECF.h>
#include "SymbRegEvalOp.h"


int main(int argc, char **argv)
{
	StateP state (new State);

	// set the evaluation operator
	state->setEvalOp(new GEPSymbRegEvalOp);

	// initialize & run
	state->initialize(argc, argv);

	// optional: read individual from file
	//XMLNode xInd = XMLNode::parseFile("./ind.xml", "Individual");
	//IndividualP ind = (IndividualP) new Individual(state);
	//ind->read(xInd);
	//ECF_LOG(state, 3, "loaded individual:\n" + ind->toString());

	state->run();
	return 0;
}
