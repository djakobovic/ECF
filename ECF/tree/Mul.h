#ifndef Mul_h
#define Mul_h
#include "Primitive.h"

namespace Tree {
namespace Primitives {

/**
 * \ingroup tree genotypes primitives
 * \brief Mul function primitive (Tree genotype)
 */
template <class T>
class MulT : public Primitive
{
public:
	MulT(void);
	void execute(void* result, Tree& tree);
	~MulT(void);
};
typedef MulT<double> Mul;


template <class T>
MulT<T>::MulT(void)
{
	nArguments_ = 2;
	name_ = "*";
	complementName_ = "/"; 
}


template <class T>
MulT<T>::~MulT(void)
{	}


template <class T>
void MulT<T>::execute(void* result, Tree &tree)
{
	T first, second;
	T& mul = *(T*)result;
    getNextArgument(&first, tree);
    getNextArgument(&second, tree);
    mul = first * second;
}

}
}

#endif
