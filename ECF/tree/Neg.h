#ifndef Neg_h
#define Neg_h
#include "Primitive.h"

namespace Tree {
namespace Primitives {

/**
 * \ingroup tree genotypes primitives
 * \brief Neg function primitive (Tree genotype)
 *
 * neg(x) = (x < 0) ? x : 0
 */
template <class T>
class NegT : public Primitive
{
public:
	NegT();
	void execute(void* result, Tree& tree);
	~NegT();
};
typedef NegT<double> Neg;


template <class T>
NegT<T>::NegT()
{
	nArguments_ = 1;
	name_ = "neg";
	complementName_ = "pos";
}


template <class T>
NegT<T>::~NegT()
{	}


template <class T>
void NegT<T>::execute(void* result, Tree& tree)
{
	T arg;
	T& neg = *(T*)result;
    getNextArgument(&arg, tree);
	neg = arg < 0 ? arg : 0;
}

}
}

#endif