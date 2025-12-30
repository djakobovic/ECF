#include <ECF/ECF.h>
#include "OneMaxEvalOp.h"


class MyEvalOp : public EvaluateOp
{
public:
	StateP state_;

	bool initialize(StateP state)
	{
		state_ = state;
		return true;
	}
	FitnessP evaluate(IndividualP individual)
	{
		DemeP myDeme = state_->getPopulation()->getLocalDeme();
		for (IndividualP ind : *myDeme) {
			std::cout << ind->index << ": " << ind->toString() << std::endl;
		}
		return (FitnessP)(new FitnessMin);
	}
};


int main(int argc, char **argv)
{
	StateP state (new State);

	// set the evaluation operator
	//state->setEvalOp(new OneMaxEvalOp);
	state->setEvalOp(new MyEvalOp);

	state->initialize(argc, argv);
	state->run();

	return 0;
}
