#ifndef CGPMax_h
#define CGPMax_h
#include "Function.h"

namespace Cartesian
{
	template <class T>
	class MaxT : public Function
	{
	public:
		MaxT();
		~MaxT();

		void evaluate(std::vector<T>& inputs, T& result);
	};

	typedef MaxT<double> Max;

	template <class T>
	MaxT<T>::MaxT()
	{
		name_ = "max";
		nArguments_ = 2;
	}


	template <class T>
	MaxT<T>::~MaxT()
	{}

	template <class T>
	void MaxT<T>::evaluate(std::vector<T>& inputs, T& result)
	{
		result = std::max(inputs[0], inputs[1]);
	}

}

#endif /* Add_h */