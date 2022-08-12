#ifndef MOFUNCTIONMINEVALOP_H_
#define MOFUNCTIONMINEVALOP_H_


// primjer multi-objective evaluacijskog operatora
class MOEvalOp : public EvaluateOp 
{
public:
	FitnessP evaluate(IndividualP individual)
	{
		// stvori novi MOFitness u kojega cemo staviti nekoliko osnovnih Fitness objekata
		MOFitnessP fitness = static_cast<MOFitnessP> (new MOFitness);

		FloatingPointP gen = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (individual->getGenotype());
		// FloatingPoint::FloatingPoint* gen = static_cast<FloatingPoint::FloatingPoint*> (individual->getGenotype().get());

		
		double x1 = gen->realValue.at(0);
		double x2 = gen->realValue.at(1);

		double f1 = pow(x1,2) + pow(x2,2); // unimodalna funkcija minimum u (0,0)
		double f2 = pow(x1-5,2) + pow(x2-5,2); // unimodalna funkcija minimum u (5,5)

		fitness->push_back((FitnessP) new FitnessMin);
		fitness->back()->setValue(f1);

		fitness->push_back((FitnessP) new FitnessMin);
		fitness->back()->setValue(f2);

		return fitness;
	}
};


#endif /* MOFUNCTIONMINEVALOP_H_ */
