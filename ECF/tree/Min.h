#ifndef Min_h
#define Min_h
#include "Primitive.h"

#include <algorithm>

namespace Tree {
namespace Primitives {

/**
 * \ingroup tree genotypes primitives
 * \brief Min function primitive (Tree genotype)
 */
template <class T>
class MinT : public Primitive
{
public:
	MinT();
	void execute(void* result, Tree& tree);
	~MinT();
};
typedef MinT<double> Min;


template <class T>
MinT<T>::MinT()
{
	nArguments_ = 2;
	name_ = "min";
	complementName_ = "max";
}


template <class T>
MinT<T>::~MinT()
{	}


template <class T>
void MinT<T>::execute(void* result, Tree& tree)
{
	T first, second;
	T& min = *(T*)result;
	getNextArgument(&first, tree);
	getNextArgument(&second, tree);
	min = std::min(first, second);
}

}
}

#endif
