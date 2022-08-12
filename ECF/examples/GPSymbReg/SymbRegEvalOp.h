#ifndef SymbRegEvalOp_h
#define SymbRegEvalOp_h

class SymbRegEvalOp : public EvaluateOp
{
public:
// metoda za evaluaciju
	FitnessP evaluate(IndividualP individual);
	void registerParameters(StateP);
	bool initialize(StateP);
	std::vector<double> domain;
	std::vector<double> codomain;
	uint nSamples;
	LoggerP logger;
};
typedef boost::shared_ptr<SymbRegEvalOp> SymbRegEvalOpP;

#endif // SymbRegEvalOp_h
