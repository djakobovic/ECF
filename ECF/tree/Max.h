#ifndef Max_h
#define Max_h
#include "Primitive.h"

#include <algorithm>

namespace Tree {
namespace Primitives {

/**
 * \ingroup tree genotypes primitives
 * \brief Max function primitive (Tree genotype)
 */
template <class T>
class MaxT : public Primitive
{
public:
	MaxT();
	void execute(void* result, Tree& tree);
	~MaxT();
};
typedef MaxT<double> Max;


template <class T>
MaxT<T>::MaxT()
{
	nArguments_ = 2;
	name_ = "max";
	complementName_ = "min";
}


template <class T>
MaxT<T>::~MaxT()
{	}


template <class T>
void MaxT<T>::execute(void* result, Tree& tree)
{
	T first, second;
	T& max = *(T*)result;
	getNextArgument(&first, tree);
	getNextArgument(&second, tree);
	max = std::max(first, second);
}

}
}

#endif
