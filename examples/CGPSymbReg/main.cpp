#include <ECF/ECF.h>
#include "SymbRegEvalOp.h"


int main(int argc, char **argv) 
{
	StateP state(new State);

	state->setEvalOp(new CGPSymbRegEvalOp);

	if (!state->initialize(argc, argv))
		return 1;
	state->run();

	return 0;

	// optional block: write best individual to 'best.txt'
	std::vector<IndividualP> hof = state->getHoF()->getBest();
	IndividualP best = hof[0];
	ofstream outfile("./best.txt");
	outfile << best->toString();
	outfile.close();

	// optional block: read individual from 'best.txt' (for subsequent simulation)
	XMLNode xInd = XMLNode::parseFile("./best.txt", "Individual");
	IndividualP ind = (IndividualP) new Individual(state);
	ind->read(xInd); 
	state->getAlgorithm()->evaluate(ind);
	std::cout << ind->toString() << "\n";

	return 0;
}