#include "../ECF_base.h"
#include "Cos.h"
#include <cmath>

namespace Tree {
namespace Primitives {

Cos::Cos(void)
{
	nArguments_ = 1;
	name_ = "cos";
	complementName_ = "sin";
}


Cos::~Cos(void)
{	}


void Cos::execute(void* result, Tree &tree)
{
	double& arg = *(double*)result;
    getNextArgument(&arg, tree);
	// radians as argument
    arg = cos(arg);
}

}
}