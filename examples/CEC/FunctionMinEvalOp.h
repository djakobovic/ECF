#ifndef CECFUNCTIONMINEVALOP_H_
#define CECFUNCTIONMINEVALOP_H_

#include <cmath>


/**
 * \defgroup cec CEC 2014 function minimization example (from http://www.ntu.edu.sg/home/EPNSugan/index_files/CEC2014/CEC2014.htm)
 * \ingroup examples
 * \brief CEC 2014 Real-Parameter Single Objective Optimization - using FloatingPoint or Binary genotype
 *
 * \par Description
 * Tries to find the (global) minimum of a continuous multidimensional function.
 * The example uses either FloatingPoint or Binary genotype.
 * Functions are implemented in a package available at http://www.ntu.edu.sg/home/EPNSugan/index_files/CEC2014/CEC2014.htm. 
 * 
 * The shifted global optimum (defined in 'shift_data_x.txt') is randomly distributed in [-80,80]^D. 
 * Search range: [-100,100]^D.
 * The value of the optimum of all functions is equal to (functionID * 100) (see documentation, link below).
 *
 * IMPORTANT: the CEC input data folder must be in path to run the experiments. 
 * DOWNLOAD the required data and place them in "input_data" folder from http://web.mysites.ntu.edu.sg/epnsugan/PublicSite/Shared%20Documents/CEC-2014/cec14-c-code.zip
 * Since the data size is 35MB, it is NOT included in the ECF package.
 * 
 * \par Parameters (in the config file)
 * - "cec.function": defines the minimization function to be used (default: 1); valid range: 1-30
 * - with FloatingPoint genotype, a number of dimensions and the interval [lbound, ubound] must be set
 * - with Binary genotype, a number of dimensions, the interval [lbound, ubound] and precision 
 * (number of digits after the decimal point) must be set
 *
 * \par References
 * - http://www.ntu.edu.sg/home/EPNSugan/index_files/CEC2014/CEC2014.htm
 * - http://web.mysites.ntu.edu.sg/epnsugan/PublicSite/Shared%20Documents/CEC-2014/Definitions%20of%20%20CEC2014%20benchmark%20suite%20Part%20A.pdf 
 */


/**
 * \ingroup cec
 * \brief Function minimization evaluation class.
 */
class FunctionMinEvalOp : public EvaluateOp 
{
public:
	uint iFunction_;	//!< function index

public:
	FunctionMinEvalOp()
	{	}
	FitnessP evaluate(IndividualP individual);
	void registerParameters(StateP);
	bool initialize(StateP);
};
typedef boost::shared_ptr<FunctionMinEvalOp> FunctionMinEvalOpP;

#endif /* CECFUNCTIONMINEVALOP_H_ */
