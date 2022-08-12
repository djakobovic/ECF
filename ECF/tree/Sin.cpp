#include "../ECF_base.h"
#include <cmath>
#include "Sin.h"

namespace Tree {
namespace Primitives {

Sin::Sin(void)
{
	nArguments_ = 1;
	name_ = "sin";
	complementName_ = "cos";
}


Sin::~Sin(void)
{	}


void Sin::execute(void* result, Tree &tree)
{
	double& arg = *(double*)result;
    getNextArgument(&arg, tree);
	// argument in radians
    arg = sin(arg);
}

}
}