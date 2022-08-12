#include "ECF_base.h"
#include "ECF_macro.h"
#include <cmath>


StatCalc::StatCalc() 
{
	statNo = 0;
	statsFileName_ = "";
}


void StatCalc::registerParameters(StateP state)
{
	state->getRegistry()->registerEntry("stats.file", (voidP) (new std::string("")), ECF::STRING);
}


bool StatCalc::initialize(StateP state)
{
	state_ = state;
	average_.clear();
	stdDev_.clear();
	max_.clear();
	min_.clear();
	time_.clear();
	sampleSize_.clear();
	evaluations_.clear();
	nEvaluations_ = 0;
	lowest_ = highest_ = 0;

	if(state->getRegistry()->isModified("stats.file")) {
		voidP sptr = state->getRegistry()->getEntry("stats.file");
		statsFileName_ = *((std::string*) sptr.get());

		statsFile_.open(statsFileName_.c_str());
		if(!statsFile_) {
			throw std::string("Error: can't open stats file (") + statsFileName_ + ")";
		}
	}
	return true;
}


/**
 * Get minimum fitness value for a given generation.
 */
double StatCalc::getFitnessMin(int generation)
{
	if(generation == -1)
		generation = (int) statNo;
	return min_[generation];
}


/**
 * Get maximum fitness value for a given generation.
 */
double StatCalc::getFitnessMax(int generation)
{
	if(generation == -1)
		generation = (int) statNo;
	return max_[generation];
}


/**
 * \brief Calculate basic fitness statistics on given individual pool.
 * Add new statistic for each measure (min, max, avg, dev, time, size, low, high)
 */
bool StatCalc::operate(const std::vector<IndividualP>& pool)
{
	std::vector<double> fitnessTemp;

	sampleSize_.push_back((uint)pool.size());
	fitnessTemp.resize(pool.size());

	for(uint i = 0; i < pool.size(); i++) 
		fitnessTemp[i] = pool[i]->fitness->getValue();

	max_.push_back(fitnessTemp[0]);
	min_.push_back(fitnessTemp[0]);
	statNo = (uint) max_.size() - 1;
	double sum = 0;

	for(uint i = 0; i < fitnessTemp.size(); i++) {
		if(fitnessTemp[i] > max_[statNo]) {
			max_[statNo] = fitnessTemp[i];
		}
		if(fitnessTemp[i] < min_[statNo]) {
			min_[statNo] = fitnessTemp[i];
		}
		sum += fitnessTemp[i];
	}

	average_.push_back(sum / fitnessTemp.size());

	// check lowest and highest fitness values
	if(min_.size() > 1) {
		if(min_[statNo] < lowest_)
			lowest_ = min_[statNo];
		if(max_[statNo] > highest_)
			highest_ = max_[statNo];
	} else {
		lowest_ = min_[0];
		highest_ = max_[0];
	}

	// compute standard deviation
	if(fitnessTemp.size() > 1) {
		double numerator = 0, denominator;
		for(uint i = 0; i < fitnessTemp.size(); i++) {
			numerator += ((fitnessTemp[i] - average_[statNo]) * (fitnessTemp[i] - average_[statNo]));
		}
		denominator = (double) (fitnessTemp.size() - 1);
		stdDev_.push_back(sqrt(numerator / denominator));
	}
	else
		stdDev_.push_back(fitnessTemp[0]);

	time_.push_back(state_->getElapsedTime());

	evaluations_.push_back(nEvaluations_);

	return true;
}


/**
 * Copy latest statistics from another Stat object.
 */
void StatCalc::copyStats(StatCalcP stats)
{
	std::vector<double> exStats = stats->getStats();
	min_.push_back(exStats[0]);
	max_.push_back(exStats[1]);
	average_.push_back(exStats[2]);
	stdDev_.push_back(exStats[3]);
	sampleSize_.push_back((uint) exStats[4]);
	time_.push_back((uint) exStats[5]);
	evaluations_.push_back((uint) exStats[6]);
	lowest_ = exStats[ECF::FIT_LOW];
	highest_ = exStats[ECF::FIT_HIGH];

	nEvaluations_ = evaluations_.back();
	statNo = (uint) max_.size() - 1;
}


/**
 * Merge current statistics with additional individual set's statistics.
 */
bool StatCalc::update(std::vector<double> stats)
{
	double outMin = stats[0];
	double outMax = stats[1];
	double outAvg = stats[2];
	double outDev = stats[3];
	uint outSize = (uint) stats[4];
	uint outEval = (uint) stats[6];

	if(outMin < min_.back())
		min_.back() = outMin;
	
	if(outMax > max_.back())
		max_.back() = outMax;

	evaluations_.back() += outEval;
	nEvaluations_ = evaluations_.back();

	uint mySize = sampleSize_.back();
	if(mySize > 1) {
		sampleSize_.back() = mySize + outSize;
		double myAvg = average_.back();
		double newAvg = (myAvg * mySize + outAvg * outSize) / sampleSize_.back();
		average_.back() = newAvg;

		uint totSize = sampleSize_.back();
		double mySum = myAvg * mySize;
		double outSum = outAvg * outSize;
		double totSum = mySum + outSum;
		double mySumSqr = pow(stdDev_.back(), 2) * mySize + mySum / mySize * mySum;
		double outSumSqr = outDev * outDev * outSize + outSum / outSize * outSum;
		double newDev = sqrt((mySumSqr + outSumSqr - (totSum / totSize * totSum)) / totSize);
		stdDev_.back() = newDev;
	}
	else {
		average_.back() = outAvg;
		stdDev_.back() = outDev;
		sampleSize_.back() = outSize;
	}

	if(stats[ECF::FIT_LOW] < lowest_)
		lowest_ = stats[ECF::FIT_LOW];
	if(stats[ECF::FIT_HIGH] > highest_)
		highest_ = stats[ECF::FIT_HIGH];

	return true;
}


/**
 * Get statistic values for a given generation.
 */
std::vector<double> StatCalc::getStats(int gen)
{
	if(gen == -1)
		gen = (int) statNo;

	std::vector<double> stats;
	stats.push_back(min_[gen]);
	stats.push_back(max_[gen]);
	stats.push_back(average_[gen]);
	stats.push_back(stdDev_[gen]);
	stats.push_back((double) sampleSize_[gen]);
	stats.push_back((double) time_[gen]);
	stats.push_back((double) evaluations_[gen]);
	stats.push_back(lowest_);
	stats.push_back(highest_);

	return stats;
}


/**
 * Log statistics.
 */
void StatCalc::log(int generation)
{
	if(generation == -1)
		generation = (int) statNo;

	ECF_LOG(state_, 3, "Evaluations: " + uint2str(evaluations_[generation]) + 
		"\nStats: fitness\n\tmax: " + dbl2str(max_[generation]) + "\n\tmin: " +
		dbl2str(min_[generation]) + "\n\tavg: " + dbl2str(average_[generation]) + "\n\tstdev: " + dbl2str(stdDev_[generation]) + "\n");
}


// ispis tijeka min fitnesa - za potrebe mjerenja
void StatCalc::output(uint step)
{
	std::stringstream log;
	for(uint i = 0; i < min_.size(); i += step)
		log << i << "\t" << min_[i] << std::endl;
	ECF_LOG(state_, 2, log.str());
}