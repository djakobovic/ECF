//
// Primjer korisnicki definirane funkcije za stablo
// (isto kao Add, samo je znak '%')
//
#ifndef Ad_h
#define Ad_h
#include "../../tree/Primitive.h"

namespace Tree {

template <class T>
class AdT : public Primitives::Primitive
{
public:
	AdT();
	void execute(void* result, Tree& tree);
	~AdT();
};
typedef AdT<double> Ad;

template <class T>
AdT<T>::AdT()
{
	nArguments_ = 2;
	name_ = "%";
}

template <class T>
AdT<T>::~AdT()
{	}

template <class T>
void AdT<T>::execute(void* result, Tree& tree)
{
	T prvi, drugi;
	T& zbroj = *(T*)result;
    getNextArgument(&prvi, tree);
    getNextArgument(&drugi, tree);
    zbroj = prvi + drugi;
}

}

#endif