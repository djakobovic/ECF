#ifndef CGPCos_h
#define CGPCos_h
#include "Function.h"

namespace Cartesian
{
	template <class T>
	class CosT: public Function
	{
	public:
		CosT();
		~CosT();

		void evaluate(std::vector<T>& inputs, T& result);
	};

	typedef CosT<double> Cos;
	template <class T>
	CosT<T>::CosT()
	{
		name_ = "cos";
		nArguments_ = 1;
	}

	template <class T>
	CosT<T>::~CosT()
	{}

	template <class T>
	void CosT<T>::evaluate(std::vector<T> &inputs, T &result)
	{
		result = cos(inputs[0]);
	}
}
#endif //Cos_h
