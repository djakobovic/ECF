#ifndef Div_h
#define Div_h
#include <cmath>
#include "Primitive.h"

namespace Tree {
namespace Primitives {

const double MIN = 0.000000001; // protected division parameter

/**
 * \ingroup tree genotypes primitives
 * \brief Div function primitive (Tree genotype)
 */
template <class T>
class DivT : public Primitive
{
public:
	DivT(void);
	void execute(void* result, Tree& tree);
	~DivT(void);
};
typedef DivT<double> Div;


template <class T>
DivT<T>::DivT(void)
{
	nArguments_ = 2;
	name_ = "/";
	complementName_ = "*";
}


template <class T>
DivT<T>::~DivT(void)
{	}


template <class T>
void DivT<T>::execute(void* result, Tree &tree)
{
	T first, second;
	T& division = *(T*)result;
    getNextArgument(&first, tree);
    getNextArgument(&second, tree);

	// T must be auto castable to double!
	division = fabs(second) > MIN ? first / second : 1.;
}

}
}

#endif
