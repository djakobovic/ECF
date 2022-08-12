#ifndef aproksimacija_h
#define aproksimacija_h

class AproxEvalOp : public EvaluateOp
{
public:
// metoda za evaluaciju
	FitnessP evaluate(IndividualP individual);
	bool initialize(StateP);
	void registerParameters(StateP state);
	double vut(int rbr_tocke);

	std::vector<double> x;
	std::vector<double> y;
	uint nTocaka;
};
typedef boost::shared_ptr<AproxEvalOp> AproxEvalOpP;

#endif // aproskimacija_h
