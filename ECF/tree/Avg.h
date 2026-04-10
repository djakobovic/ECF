#ifndef Avg_h
#define Avg_h
#include "Primitive.h"

namespace Tree {
namespace Primitives {

/**
 * \ingroup tree genotypes primitives
 * \brief Avg function primitive (Tree genotype)
 */
class Avg : public Primitive
{
public:
	Avg(void)
	{
		nArguments_ = 2;
		name_ = "avg";
	}

	~Avg(void)
	{	}

	void execute(void* result, Tree& tree)
	{
		double& avg = *(double*)result;
		double first, second;
		getNextArgument(&first, tree);
		getNextArgument(&second, tree);
		avg = (first + second) / 2;
	}
};

}
}

#endif 