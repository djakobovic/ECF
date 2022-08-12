#ifndef Pos_h
#define Pos_h
#include "Primitive.h"

namespace Tree {
namespace Primitives {

/**
 * \ingroup tree genotypes primitives
 * \brief Pos function primitive (Tree genotype)
 *
 * pos(x) = (x > 0) ? x : 0
 */
template <class T>
class PosT : public Primitive
{
public:
	PosT();
	void execute(void* result, Tree& tree);
	~PosT();
};
typedef PosT<double> Pos;


template <class T>
PosT<T>::PosT()
{
	nArguments_ = 1;
	name_ = "pos";
	complementName_ = "neg";
}


template <class T>
PosT<T>::~PosT()
{	}


template <class T>
void PosT<T>::execute(void* result, Tree& tree)
{
	T arg;
	T& pos = *(T*)result;
    getNextArgument(&arg, tree);
	pos = arg > 0 ? arg : 0;
}

}
}

#endif