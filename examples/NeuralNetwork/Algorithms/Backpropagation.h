#ifndef Backpropagation_h
#define Backpropagation_h
#include <ecf/ECF.h>

class Backpropagation : public Algorithm
{
public:
	Backpropagation();
	bool advanceGeneration(StateP state, DemeP deme);
	bool initialize(StateP state);
	void registerParameters(StateP state);

protected:
	uint learningRate;

	static bool compare(IndividualP first, IndividualP second)
	{	return first->fitness->isBetterThan(second->fitness);	}

private:
	double learningRate_;
	
};
typedef std::shared_ptr<Backpropagation> BackpropagationP;

#endif // Evolution_Strategy_h
