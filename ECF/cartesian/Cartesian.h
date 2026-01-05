#ifndef Cartesian_h_h
#define Cartesian_h_h

// genotype
#include "Cartesian_c.h"

// function set - put all avaliable function names and objects there
#include "FunctionSet.h"

// available functions
#include "Add.h"
#include "Sub.h"
#include "Mul.h"
#include "Div.h"
#include "Sin.h"
#include "Cos.h"
#include "Sqrt.h"
#include "Nlog.h"
#include "Exp.h"
#include "Step.h"
#include "Pos.h"
#include "Min.h"
#include "Max.h"

// crossover operators
#include "CartesianCrxOnePoint.h"
#include "CartesianCrxHalfUniform.h"
#include "CartesianCrxUniform.h"

// mutation operators
#include "CartesianMutOnePoint.h"
#include "CartesianMutNonSilent.h"
#include "CartesianMutSilent.h"
#include "CartesianMutNewParameterLess.h"

#endif /* Cartesian_h_h */