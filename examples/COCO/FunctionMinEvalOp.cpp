#include <ECF/ECF.h>
#include "FunctionMinEvalOp.h"

// COCO
#include "./bbob/bbobStructures.h" /* Include all declarations for BBOB calls */
#include <string.h>
extern int enableOutput;


void FunctionMinEvalOp::registerParameters(StateP state)
{
	state->getRegistry()->registerEntry("coco.function", (voidP) (new uint(1)), ECF::UINT,
		"sets the COCO objective function (default: 1)");
	state->getRegistry()->registerEntry("coco.instance", (voidP) (new uint(1)), ECF::UINT,
		"set instance no. for COCO function (default: 1)");
	state->getRegistry()->registerEntry("coco.enableoutput", (voidP) (new uint(0)), ECF::UINT,
		"enable COCO folder output (default: 0/no)");
	state->getRegistry()->registerEntry("coco.folder", (voidP) (new std::string("")), ECF::STRING,
		"sets the COCO output folder (default: none)");
	state->getRegistry()->registerEntry("coco.algorithm", (voidP) (new std::string("")), ECF::STRING,
		"set algorithm name for post processing (default: none)");
	state->getRegistry()->registerEntry("coco.comments", (voidP) (new std::string("")), ECF::STRING,
		"set algorithm description for post processing (default: none)");
}


bool FunctionMinEvalOp::initialize(StateP state)
{
	if(experimentMode_)
		return true;

	// if repeated runs: finalize first
	if(isCocoInitialized_) {
		fgeneric_finalize();
	}

	voidP sptr = state->getRegistry()->getEntry("coco.function"); // get parameter value
	iFunction_ = *((uint*) sptr.get()); // convert from voidP to user defined type

	sptr = state->getRegistry()->getEntry("coco.instance"); // get parameter value
	cocoInstance_ = *((uint*) sptr.get()); // convert from voidP to user defined type

	// read COCO output folder name
	if(state->getRegistry()->isModified("coco.folder")) {
		sptr = state->getRegistry()->getEntry("coco.folder");
		cocoFolder_ = *((std::string*) sptr.get());
	}

	// should COCO output be enabled (if not previously enabled manually)
	if(!enableOutput) {
		sptr = state->getRegistry()->getEntry("coco.enableoutput");
		enableOutput = *((uint*) sptr.get());
	}

	//
	// read genotype dimension from the registry; two options:
	//
	// uncomment the following line when using FloatingPoint genotype:
	sptr = state->getRegistry()->getEntry("FloatingPoint.dimension");
	// uncomment the following line when using Binary genotype:
	//sptr = state->getRegistry()->getEntry("Binary.dimension");

	uint dim = *((uint*) sptr.get());


	//
	// COCO initialization (with excerpts from exampleexperiment.c)
	//

	ParamStruct params = fgeneric_getDefaultPARAMS();
	strcpy(params.dataPath, cocoFolder_.c_str());  /* different folder for each experiment! */

    /* set DIM, funcId, instanceId to initialize BBOB fgeneric */
	params.DIM = dim;
    params.funcId = iFunction_;
	// instanceId is 1 by default
	params.instanceId = cocoInstance_;

	// algorithm name
	sptr = state->getRegistry()->getEntry("coco.algorithm");
	cocoAlgName_ = *((std::string*) sptr.get());
	strcpy(params.algName, cocoAlgName_.c_str());

	// algorithm description
	sptr = state->getRegistry()->getEntry("coco.comments");
	cocoAlgComments_ = *((std::string*) sptr.get());
	strcpy(params.comments, cocoAlgComments_.c_str());

	/* call the BBOB initialization */
    fgeneric_initialize(params);
	// get function optimal value
	coco_optimum_ = fgeneric_ftarget() - params.precision;
	isCocoInitialized_ = true;

	return true;
}


FitnessP FunctionMinEvalOp::evaluate(IndividualP individual)
{
	// evaluation creates a new fitness object using a smart pointer
	// in our case, we try to minimize the function value, so we use FitnessMin fitness (for minimization problems)
	FitnessP fitness (new FitnessMin);

	// we define FloatingPoint as the only genotype (in the configuration file)

	// uncomment the following line when using FloatingPoint genotype:
	FloatingPoint::FloatingPoint* gen = (FloatingPoint::FloatingPoint*) individual->getGenotype().get();
	// (you can also use boost smart pointers:)
	//FloatingPointP gen = boost::dynamic_pointer_cast<FloatingPoint::FloatingPoint> (individual->getGenotype());

	// alternative encoding: Binary Genotype

	// uncomment the following line when using Binary genotype:
	//Binary::Binary* gen = (Binary::Binary*) individual->getGenotype().get();
	//BinaryP gen = boost::dynamic_pointer_cast<Binary::Binary> (individual->getGenotype());

	double value = 0;


	//
	// COCO
	//
	// evaluate
	value = fgeneric_evaluate(&(gen->realValue[0]));
	// scale to zero minimum
	value -= coco_optimum_;

	fitness->setValue(value);
	return fitness;
}
