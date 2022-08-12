#ifndef ECF_base_h
#define ECF_base_h

#include <iostream>
#include <vector>
#include <cstdlib>
#include "boost/smart_ptr.hpp"
#include "xml/xmlParser.h"
#ifdef _MPI
#include "mpi.h"
#endif

typedef boost::shared_ptr<void>  voidP;
typedef unsigned int uint;

// base:
#include "ECF_macro.h"
#include "Logger.h"
#include "Registry.h"
#include "Genotype.h"
#include "Fitness.h"
#include "Individual.h"
#include "Deme.h"
#include "Population.h"
#include "Crossover.h"
#include "Mutation.h"
#include "State.h"
#include "Randomizer.h"
#include "SelectionOperator.h"
#include "EvaluateOp.h"
#include "Algorithm.h"
#include "Operator.h"
#include "Communicator.h"

#endif // ECF_base_h

