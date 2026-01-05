#ifndef CGPMin_h
#define CGPMin_h
#include "Function.h"

namespace Cartesian
{
	template <class T>
	class MinT : public Function
	{
	public:
		MinT();
		~MinT();

		void evaluate(std::vector<T>& inputs, T& result);
	};

	typedef MinT<double> Min;

	template <class T>
	MinT<T>::MinT()
	{
		name_ = "min";
		nArguments_ = 2;
	}


	template <class T>
	MinT<T>::~MinT()
	{}

	template <class T>
	void MinT<T>::evaluate(std::vector<T>& inputs, T& result)
	{
		result = std::min(inputs[0], inputs[1]);
	}

}

#endif /* Add_h */