#ifndef CGPSqrt_h
#define CGPSqrt_h
#include "Function.h"
namespace Cartesian
{
	template <class T>
	class SqrtT : public Function
	{
	public:
		SqrtT();
		~SqrtT();

		void evaluate(std::vector<T>& inputs, T& result);
	};

	typedef SqrtT<double> Sqrt;

	template <class T>
	SqrtT<T>::SqrtT()
	{
		name_ = "sqrt";
		nArguments_ = 1;
	}

	template  <class T>
	SqrtT<T>::~SqrtT()
	{}

	template  <class T>
	void SqrtT<T>::evaluate(std::vector<T> &inputs, T &result)
	{
		result = inputs[0];
		if(inputs[0] > 0)
			result = sqrt(inputs[0]);
	}
}
#endif //Sqrt_h
