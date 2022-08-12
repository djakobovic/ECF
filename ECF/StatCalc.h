#ifndef STATCALC_H_
#define STATCALC_H_


namespace ECF
{

// constants for statistic values reference
enum stats
	{ FIT_MIN, FIT_MAX, FIT_AVG, FIT_DEV, STAT_SIZE, STAT_TIME, STAT_EVAL, FIT_LOW, FIT_HIGH };

}


/**
 * \ingroup evol population
 * \brief Statistics calculation class.
 */
class StatCalc : public Operator
{
private:
	uint statNo;
	std::vector<double> average_;        //!< average fitness values for each generation
	std::vector<double> stdDev_;         //!< deviation of fitness values for each generation
	std::vector<double> max_;            //!< maximum fitness values for each generation
	std::vector<double> min_;            //!< minimum fitness values for each generation
	std::vector<uint> sampleSize_;
	std::vector<uint> time_;
	std::vector<uint> evaluations_;      //!< total number of evaluations for each generation
	double lowest_;                      //!< lowest recorded fitness value
	double highest_;                     //!< highest recorded fitness value
	uint nEvaluations_;                  //!< total number of evaluations
	StateP state_;
	std::string statsFileName_;
	std::ofstream statsFile_;

public:
	StatCalc();

	bool operate(StateP)
	{	return false;	}

	bool operate(const std::vector<IndividualP>&);
	void registerParameters(StateP);
	bool initialize(StateP);
	bool update(std::vector<double>);
	void log(int generation = -1);
	std::vector<double> getStats(int generation = -1);
	void output(uint step);
	void copyStats(StatCalcP);

	double getFitnessMin(int generation = -1);
	double getFitnessMax(int generation = -1);

	/**
	 * increase and return total number of fitness evaluations
	 */
	uint increaseEvaluations(uint eval = 1)
	{
		nEvaluations_ += eval;
		return nEvaluations_;
	}

	/**
	 * set total number of fitness evaluations
	 */
	void setEvaluations(uint eval)
	{	nEvaluations_ = eval;	}

	/**
	 * get total number of fitness evaluations
	 */
	uint getEvaluations()
	{	return nEvaluations_;	}

	/**
	 * get lowest ever fitness vale
	 */
	double getLowestFitness()
	{	return lowest_;	}

	/**
	 * get highest ever fitness vale
	 */
	double getHighestFitness()
	{	return highest_;	}

};
typedef boost::shared_ptr<StatCalc> StatCalcP;

#endif /* STATCALC_H_ */
