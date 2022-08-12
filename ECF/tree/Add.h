#ifndef Add_h
#define Add_h
#include "Primitive.h"

namespace Tree {
namespace Primitives {

/**
 * \ingroup tree genotypes primitives
 * \brief Add function primitive (Tree genotype)
 */
template <class T>
class AddT : public Primitive
{
public:
	AddT();
	void execute(void* result, Tree& tree);
	~AddT();
};
typedef AddT<double> Add;


template <class T>
AddT<T>::AddT()
{
	nArguments_ = 2;
	name_ = "+";
	complementName_ = "-";
}


template <class T>
AddT<T>::~AddT()
{	}


template <class T>
void AddT<T>::execute(void* result, Tree& tree)
{
	T first, second;
	T& sum = *(T*)result;
	getNextArgument(&first, tree);
	getNextArgument(&second, tree);
	sum = first + second;
}

}
}

#endif