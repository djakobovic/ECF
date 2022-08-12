#ifndef TSPEVALOP_H_
#define TSPEVALOP_H_

#include <cmath>
#include <iostream>
#include <string>
#include <fstream>

class TSPEvalOp : public EvaluateOp 
{
private:
	int dimension;
	std::vector< std::vector<int> > weights;
public:
	void registerParameters(StateP);
	bool initialize(StateP);
	FitnessP evaluate(IndividualP individual);
};
typedef boost::shared_ptr<TSPEvalOp> TSPEvalOpP;

#endif /* TSPEVALOP_H_ */
