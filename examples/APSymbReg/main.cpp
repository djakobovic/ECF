#include <ECF/ECF.h>
#include "SymbRegEvalOp.h"


int main(int argc, char **argv)
{
	StateP state (new State);

	// set the evaluation operator
	state->setEvalOp(new SymbRegEvalOp);

	state->initialize(argc, argv);
	state->run();

	// after the evolution: show best solution in Tree form
	std::vector<IndividualP> hof = state->getHoF()->getBest();
	IndividualP ind = hof[0];
	Tree::Tree* tree = (Tree::Tree*) ((Tree::APGenotype*) ind->getGenotype().get())->convertToPhenotype();
	std::cout << tree->toString();

	return 0;
}
