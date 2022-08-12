#ifndef Sub_h
#define Sub_h
#include "Primitive.h"

namespace Tree {
namespace Primitives {

/**
 * \ingroup tree genotypes primitives
 * \brief Sub function primitive (Tree genotype)
 */
template <class T>
class SubT : public Primitive
{
public:
	SubT(void);
	void execute(void* result, Tree& tree);
	~SubT(void);
};
typedef SubT<double> Sub;

template <class T>
SubT<T>::SubT(void)
{
	nArguments_ = 2;
	name_ = "-";
	complementName_ = "+";
}


template <class T>
SubT<T>::~SubT(void)
{
}

template <class T>
void SubT<T>::execute(void* result, Tree& tree)
{   
	T prvi, drugi;
	T& res = *(T*)result;
	getNextArgument(&prvi, tree);
	getNextArgument(&drugi, tree);
	res = prvi - drugi;
}

}
}

#endif
