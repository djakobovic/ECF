#ifndef CGPDiv_h
#define CGPDiv_h
#include "Function.h"
#include <cmath>

namespace Cartesian
{
	const double MIN = 0.000000001; // protected division parameter

	template <class T>
	class DivT : public Function
	{
	public:
		DivT();
		~DivT();

		void evaluate(std::vector<T>& inputs, T& result);
	};

	typedef DivT<double> Div;

	template <class T>
	DivT<T>::DivT()
	{
		name_ = "/";
		nArguments_ = 2;
	}

	template <class T>
	DivT<T>::~DivT()
	{}

	template <class T>
	void DivT<T>::evaluate(std::vector<T> &inputs, T &result)
	{
		result = fabs(inputs[1]) > MIN ? inputs[0] / inputs[1] : inputs[0];
	}

}

#endif //Div_h
