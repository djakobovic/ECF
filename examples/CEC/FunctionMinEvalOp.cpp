#include <ECF/ECF.h>
#include "FunctionMinEvalOp.h"
#include <float.h>
#include <fstream>


// CEC externals
extern void cec14_test_func(double *, double *, int, int, int);
double *OShift,*M,*y,*z,*x_bound;
int ini_flag=0,n_flag,func_flag,*SS;


void FunctionMinEvalOp::registerParameters(StateP state)
{
	state->getRegistry()->registerEntry("cec.function", (voidP) (new uint(1)), ECF::UINT, 
		"Sets the CEC test function number, 1-30 (default: 1)");
}


bool FunctionMinEvalOp::initialize(StateP state)
{
	voidP sptr = state->getRegistry()->getEntry("cec.function"); // get parameter value
	iFunction_ = *((uint*) sptr.get()); // convert from voidP to user defined type

	// check function index (CEC does not abort on invalid id)
	if(iFunction_ < 1 || iFunction_ > 30) {
		ECF_LOG_ERROR(state, "CEC EvalOp: There are only 30 test functions in this test suite!");
		throw("");
	}

	// check for CEC input files
	std::string fileName("input_data/M_" + uint2str(iFunction_) + "_D2.txt");
	ifstream m_file(fileName.c_str());
	if (!m_file) {
		std::string msg("\nError: the CEC input data folder must be in path to run the experiments.\n");
		msg += "DOWNLOAD the required data and place them in \"input_data\" folder from http://web.mysites.ntu.edu.sg/epnsugan/PublicSite/Shared%20Documents/CEC-2014/cec14-c-code.zip.\n";
		msg += "Since the data size is 35MB, it is NOT included in the ECF package.";
		ECF_LOG_ERROR(state, msg);
		throw("");
	}

	return true;
}


FitnessP FunctionMinEvalOp::evaluate(IndividualP individual)
{
	// evaluation creates a new fitness object using a smart pointer
	// in our case, we try to minimize the function value, so we use FitnessMin fitness (for minimization problems)
	FitnessP fitness (new FitnessMin);

	// we define FloatingPoint as the only genotype (in the configuration file)
	FloatingPoint::FloatingPoint* gen = (FloatingPoint::FloatingPoint*) individual->getGenotype().get();
	// (you can also use boost smart pointers:)
	//FloatingPointP gen = boost::dynamic_pointer_cast<FloatingPoint::FloatingPoint> (individual->getGenotype());

	// alternative encoding: Binary Genotype
	//Binary::Binary* gen = (Binary::Binary*) individual->getGenotype().get();
	//BinaryP gen = boost::dynamic_pointer_cast<Binary::Binary> (individual->getGenotype());

	// we implement the fitness function 'as is', without any translation
	// the number of variables is read from the genotype itself (size of 'realValue' vactor)
	double realTemp = 0, value = 0;

	int n = (int) gen->realValue.size();

	// call external CEC 2014 evaluator
	cec14_test_func(&gen->realValue[0], &value, n, 1, iFunction_);

	fitness->setValue(value);
	return fitness;
}
